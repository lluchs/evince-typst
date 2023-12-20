// Code from typst-cli
mod args;
mod compile;
mod download;
mod fonts;
mod package;
mod watch;
mod world;

use std::cell::Cell;
use std::ffi::{c_char, CStr, CString, OsStr};
use std::io::{self, IsTerminal, Write};
use std::os::unix::prelude::OsStrExt;
use std::path::{Path, PathBuf};
use std::process::ExitCode;

use clap::Parser;
use codespan_reporting::term::{self, termcolor};
use once_cell::sync::Lazy;
use termcolor::{ColorChoice, WriteColor};
use typst::model::Document;
use typst::visualize::Color;
use world::SystemWorld;

use crate::args::{CliArguments, Command};

thread_local! {
    /// The CLI's exit code.
    static EXIT: Cell<ExitCode> = Cell::new(ExitCode::SUCCESS);
}

/// The parsed commandline arguments.
static ARGS: Lazy<CliArguments> = Lazy::new(CliArguments::parse);

/// Get stderr with color support if desirable.
fn color_stream() -> termcolor::StandardStream {
    termcolor::StandardStream::stderr(if std::io::stderr().is_terminal() {
        ColorChoice::Auto
    } else {
        ColorChoice::Never
    })
}

/// Ensure a failure exit code.
fn set_failed() {
    EXIT.with(|cell| cell.set(ExitCode::FAILURE));
}

/// Used by `args.rs`.
fn typst_version() -> &'static str {
    "0.10.0"
}

// C API

#[no_mangle]
pub unsafe extern "C" fn typst_load(path: *const c_char) -> *mut Document {
    let input_path = Path::new(OsStr::from_bytes(CStr::from_ptr(path).to_bytes())).to_path_buf();
    let args = args::SharedArgs {
        input: input_path,
        root: None,
        font_paths: Vec::new(),
        diagnostic_format: args::DiagnosticFormat::Human,
    };
    let mut world = SystemWorld::new(&args).expect("could not create typst world");
    let mut tracer = typst::eval::Tracer::new();
    let result = typst::compile(&mut world, &mut tracer);
    let warnings = tracer.warnings();

    match result {
        Ok(document) => {
            let boxed = Box::new(document);
            Box::leak(boxed)
        }
        Err(errors) => {
            compile::print_diagnostics(&world, &errors, &warnings, args.diagnostic_format).unwrap();
            std::ptr::null_mut()
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn typst_free(doc: *mut Document) {
    std::mem::drop(Box::from_raw(doc));
}

#[no_mangle]
pub unsafe extern "C" fn typst_get_n_pages(doc: *const Document) -> usize {
    (*doc).pages.len() as usize
}

#[no_mangle]
pub unsafe extern "C" fn typst_get_page_size_pt(
    doc: *const Document,
    page: usize,
    width: *mut f64,
    height: *mut f64,
) {
    let size = (*doc).pages[page].size();
    *width = size.x.to_pt();
    *height = size.y.to_pt();
}

#[no_mangle]
pub unsafe extern "C" fn typst_get_page_size_mm(
    doc: *const Document,
    page: usize,
    width: *mut f64,
    height: *mut f64,
) {
    let size = (*doc).pages[page].size();
    *width = size.x.to_mm();
    *height = size.y.to_mm();
}

/// Free a string.
#[no_mangle]
pub unsafe extern "C" fn typst_free_string(str: *mut c_char) {
    std::mem::drop(CString::from_raw(str))
}

/// Get the document's title. Returns null if there is no title.
/// Call `typst_free_string()` to free the returned pointer.
#[no_mangle]
pub unsafe extern "C" fn typst_get_title(doc: *const Document) -> *const c_char {
    if let Some(title) = &(*doc).title {
        CString::new(title.as_str())
            .expect("CString::new failed")
            .into_raw()
    } else {
        std::ptr::null()
    }
}

// TODO: author, keywords, date

/// Render a page into an ARGB32 buffer.
#[no_mangle]
pub unsafe extern "C" fn typst_render(
    doc: *const Document,
    page: usize,
    width: usize,
    height: usize,
    surface: *mut u8,
) {
    let frame = &(*doc).pages[page];
    let pixel_per_pt =
        ((width + height) as f64 / (frame.width().to_pt() + frame.height().to_pt())) as f32;
    let pixmap = typst_render::render(frame, pixel_per_pt, Color::WHITE);
    assert_eq!(pixmap.width() as usize, width);
    assert_eq!(pixmap.height() as usize, height);
    let mut i: isize = 0;
    for pixel in pixmap.pixels() {
        if cfg!(target_endian = "little") {
            *surface.offset(i) = pixel.blue();
            *surface.offset(i + 1) = pixel.green();
            *surface.offset(i + 2) = pixel.red();
            *surface.offset(i + 3) = pixel.alpha();
        } else {
            *surface.offset(i) = pixel.alpha();
            *surface.offset(i + 1) = pixel.red();
            *surface.offset(i + 2) = pixel.green();
            *surface.offset(i + 3) = pixel.blue();
        }
        i += 4;
    }
}
