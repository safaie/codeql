// generated by codegen, do not edit
import codeql.rust.elements
import TestUtils

query predicate instances(BoxPat x) { toBeTested(x) and not x.isUnknown() }

query predicate getPat(BoxPat x, Pat getPat) {
  toBeTested(x) and not x.isUnknown() and getPat = x.getPat()
}
