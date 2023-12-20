// Code from typst-cli
mod args;
mod compile;
mod download;
mod fonts;
mod package;
mod watch;
mod world;

use std::cell::Cell;
use std::process::ExitCode;
use std::io::{self, IsTerminal, Write};

use clap::Parser;
use codespan_reporting::term::{self, termcolor};
use once_cell::sync::Lazy;
use termcolor::{ColorChoice, WriteColor};

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
