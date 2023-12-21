use evince_typst::*;
use std::ffi::{CStr, CString};

#[test]
fn rects_typ() {
    let filename = CString::new("tests/rects.typ").unwrap();
    let doc = unsafe { typst_load(filename.as_ptr()) };
    assert!(!doc.is_null(), "loading succeeds");

    assert_eq!(
        unsafe { typst_get_n_pages(doc) },
        1,
        "number of pages is correct"
    );

    let mut width = 0f64;
    let mut height = 0f64;
    unsafe { typst_get_page_size_pt(doc, 0, &mut width, &mut height) };
    assert_eq!(width, 283.465f64, "page width in pt is correct");
    assert_eq!(height, 425.1975f64, "page height in pt is correct");
    unsafe { typst_get_page_size_mm(doc, 0, &mut width, &mut height) };
    assert_eq!(width, 100f64, "page width in mm is correct");
    assert_eq!(height, 150f64, "page height in mm is correct");

    let title = unsafe { typst_get_title(doc) };
    assert_eq!(
        unsafe { CStr::from_ptr(title).to_str().unwrap() },
        "Rectangles (and a circle)"
    );
    unsafe { typst_free_string(title) };

    // TODO: Test rendering

    unsafe { typst_free(doc) };
}
