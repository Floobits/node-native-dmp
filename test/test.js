/**
 * Test Harness for Diff Match and Patch
 *
 * Copyright 2006 Google Inc.
 * http://code.google.com/p/google-diff-match-patch/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

var dmp = require("../build/Release/dmp");
var assert = require('assert');

/**
 * The data structure representing a diff is an array of tuples:
 * [[DIFF_DELETE, 'Hello'], [DIFF_INSERT, 'Goodbye'], [DIFF_EQUAL, ' world.']]
 * which means: delete 'Hello', add 'Goodbye' and keep ' world.'
 */
var DIFF_DELETE = -1;
var DIFF_INSERT = 1;
var DIFF_EQUAL = 0;

function assertEquals(msg, v1, v2) {
  if (typeof v2 == 'undefined') {
    // msg is optional.
    v2 = v1;
    expected = msg;
    msg = 'Expected: \'' + v2 + '\' Actual: \'' + v1 + '\'';
  }
  v1 = JSON.stringify(v1);
  v2 = JSON.stringify(v2);
  console.log(v1);
  console.log(v2);
  assert.equal(v1, v2, msg);
}

function assertTrue(msg, actual) {
  if (typeof actual == 'undefined') {
    // msg is optional.
    actual = msg;
    assertEquals(true, actual);
  } else {
    assertEquals(msg, true, actual);
  }
}

function assertFalse(msg, actual) {
  if (typeof actual == 'undefined') {
    // msg is optional.
    actual = msg;
    assertEquals(false, actual);
  } else {
    assertEquals(msg, false, actual);
  }
}

// If expected and actual are the equivalent, pass the test.
function assertEquivalent(msg, expected, actual) {
  if (typeof actual == 'undefined') {
    // msg is optional.
    actual = expected;
    expected = msg;
    msg = 'Expected: \'' + expected + '\' Actual: \'' + actual + '\'';
  }
  if (_equivalent(expected, actual)) {
    assertEquals(msg, String.toString(expected), String.toString(actual));
  } else {
    assertEquals(msg, expected, actual);
  }
}

// Are a and b the equivalent? -- Recursive.
function _equivalent(a, b) {
  var p;

  if (a == b) {
    return true;
  }
  if (typeof a == 'object' && typeof b == 'object' && a !== null && b !== null) {
    if (a.toString() != b.toString()) {
      return false;
    }
    for (p in a) {
      if (!_equivalent(a[p], b[p])) {
        return false;
      }
    }
    for (p in b) {
      if (!_equivalent(a[p], b[p])) {
        return false;
      }
    }
    return true;
  }
  return false;
}


function diff_rebuildtexts(diffs) {
  // Construct the two texts which made up the diff originally.
  var text1 = '';
  var text2 = '';
  for (var x = 0; x < diffs.length; x++) {
    if (diffs[x][0] != DIFF_INSERT) {
      text1 += diffs[x][1];
    }
    if (diffs[x][0] != DIFF_DELETE) {
      text2 += diffs[x][1];
    }
  }
  return [text1, text2];
}


function testPatchMake() {
  // Null case.
  var patches = dmp.patch_make('', '');
  assertEquals('', patches);

  var text1 = 'The quick brown fox jumps over the lazy dog.';
  var text2 = 'That quick brown fox jumped over a lazy dog.';
  // Text2+Text1 inputs.
  var expectedPatch = '@@ -1,8 +1,7 @@\n Th\n-at\n+e\n  qui\n@@ -21,17 +21,18 @@\n jump\n-ed\n+s\n  over \n-a\n+the\n  laz\n';
  // The second patch must be "-21,17 +21,18", not "-22,17 +21,18" due to rolling context.
  patches = dmp.patch_make(text2, text1);
  assertEquals(expectedPatch, patches);

  // Text1+Text2 inputs.
  expectedPatch = '@@ -1,11 +1,12 @@\n Th\n-e\n+at\n  quick b\n@@ -22,18 +22,17 @@\n jump\n-s\n+ed\n  over \n-the\n+a\n  laz\n';
  patches = dmp.patch_make(text1, text2);
  assertEquals(expectedPatch, patches);

  // Character encoding.
  patches = dmp.patch_make('`1234567890-=[]\\;\',./', '~!@#$%^&*()_+{}|:"<>?');
  assertEquals('@@ -1,21 +1,21 @@\n-%601234567890-=%5B%5D%5C;\',./\n+~!@#$%25%5E&*()_+%7B%7D%7C:%22%3C%3E?\n', patches);

  // Character decoding.
  // diffs = [[DIFF_DELETE, '`1234567890-=[]\\;\',./'], [DIFF_INSERT, '~!@#$%^&*()_+{}|:"<>?']];
  // assertEquivalent(diffs, dmp.patch_fromText('@@ -1,21 +1,21 @@\n-%601234567890-=%5B%5D%5C;\',./\n+~!@#$%25%5E&*()_+%7B%7D%7C:%22%3C%3E?\n')[0].diffs);

  // Long string with repeats.
  text1 = '';
  for (var x = 0; x < 100; x++) {
    text1 += 'abcdef';
  }
  text2 = text1 + '123';
  expectedPatch = '@@ -573,28 +573,31 @@\n cdefabcdefabcdefabcdefabcdef\n+123\n';
  patches = dmp.patch_make(text1, text2);
  assertEquals(expectedPatch, patches);

  // Test null inputs.
  try {
    dmp.patch_make(null);
    assertEquals(Error, null);
  } catch (e) {
    // Exception expected.
  }
}

function testPatchApply() {
  dmp.Match_Distance = 1000;
  dmp.Match_Threshold = 0.5;
  dmp.Patch_DeleteThreshold = 0.5;
  // Null case.
  var patches = dmp.patch_make('', '');
  var results = dmp.patch_apply(patches, 'Hello world.');
  assertEquivalent(['Hello world.', []], results);

  // Null case, buffer.
  patches = dmp.patch_make('', '');
  results = dmp.patch_apply(patches, new Buffer('Hello world.'));
  assertEquivalent([new Buffer('Hello world.'), []], results);

  // Exact match.
  patches = dmp.patch_make('The quick brown fox jumps over the lazy dog.', 'That quick brown fox jumped over a lazy dog.');
  results = dmp.patch_apply(patches, 'The quick brown fox jumps over the lazy dog.');
  assertEquivalent(['That quick brown fox jumped over a lazy dog.', [true, true]], results);

  // Partial match.
  results = dmp.patch_apply(patches, 'The quick red rabbit jumps over the tired tiger.');
  assertEquivalent(['That quick red rabbit jumped over a tired tiger.', [true, true]], results);

  // Failed match.
  results = dmp.patch_apply(patches, 'I am the very model of a modern major general.');
  assertEquivalent(['I am the very model of a modern major general.', [false, false]], results);

  // Big delete, small change.
  patches = dmp.patch_make('x1234567890123456789012345678901234567890123456789012345678901234567890y', 'xabcy');
  results = dmp.patch_apply(patches, 'x123456789012345678901234567890-----++++++++++-----123456789012345678901234567890y');
  assertEquivalent(['xabcy', [true, true]], results);

  // Big delete, big change 1.
  patches = dmp.patch_make('x1234567890123456789012345678901234567890123456789012345678901234567890y', 'xabcy');
  results = dmp.patch_apply(patches, 'x12345678901234567890---------------++++++++++---------------12345678901234567890y');
  assertEquivalent(['xabc12345678901234567890---------------++++++++++---------------12345678901234567890y', [false, true]], results);

  // Big delete, big change 2.
  // dmp.Patch_DeleteThreshold = 0.6;
  dmp.set_Patch_DeleteThreshold(0.6);
  patches = dmp.patch_make('x1234567890123456789012345678901234567890123456789012345678901234567890y', 'xabcy');
  results = dmp.patch_apply(patches, 'x12345678901234567890---------------++++++++++---------------12345678901234567890y');
  assertEquivalent(['xabcy', [true, true]], results);
  // dmp.Patch_DeleteThreshold = 0.5;
  dmp.set_Patch_DeleteThreshold(0.5);

  // Compensate for failed patch.
  // dmp.Match_Threshold = 0.0;
  // dmp.Match_Distance = 0;
  dmp.set_Match_Threshold(0.0);
  dmp.set_Match_Distance(0);
  patches = dmp.patch_make('abcdefghijklmnopqrstuvwxyz--------------------1234567890', 'abcXXXXXXXXXXdefghijklmnopqrstuvwxyz--------------------1234567YYYYYYYYYY890');
  results = dmp.patch_apply(patches, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ--------------------1234567890');
  assertEquivalent(['ABCDEFGHIJKLMNOPQRSTUVWXYZ--------------------1234567YYYYYYYYYY890', [false, true]], results);
  // dmp.Match_Threshold = 0.5;
  // dmp.Match_Distance = 1000;
  dmp.set_Match_Threshold(0.5);
  dmp.set_Match_Distance(1000);

  // No side effects.
  patches = dmp.patch_make('', 'test');
  var patchstr = patches;
  dmp.patch_apply(patches, '');
  assertEquals(patchstr, patches);

  // No side effects with major delete.
  patches = dmp.patch_make('The quick brown fox jumps over the lazy dog.', 'Woof');
  patchstr = patches;
  dmp.patch_apply(patches, 'The quick brown fox jumps over the lazy dog.');
  assertEquals(patchstr, patches);

  // Edge exact match.
  patches = dmp.patch_make('', 'test');
  results = dmp.patch_apply(patches, '');
  assertEquivalent(['test', [true]], results);

  // Near edge exact match.
  patches = dmp.patch_make('XY', 'XtestY');
  results = dmp.patch_apply(patches, 'XY');
  assertEquivalent(['XtestY', [true]], results);

  // Edge partial match.
  patches = dmp.patch_make('y', 'y123');
  results = dmp.patch_apply(patches, 'x');
  assertEquivalent(['x123', [true]], results);

  // Binary
  patches = dmp.patch_make(new Buffer([1,2,3]), new Buffer([1,2,3,4,5]));
  results = dmp.patch_apply(patches, new Buffer([1,2,3]));
  assertEquivalent([new Buffer([1,2,3,4,5]), [true]], results);

  // Binary with null
  patches = dmp.patch_make(new Buffer([1,2,3]), new Buffer([1,2,3,0,128,4,255,5]));
  console.log("patches");
  console.log(JSON.stringify(patches));
  console.log("patches");
  results = dmp.patch_apply(patches, new Buffer([1,2,3]));
  console.log(JSON.stringify(results[0]));
  assertEquivalent([new Buffer([1,2,3,0,128,4,255,5]), [true]], results);
}

testPatchMake();
testPatchApply();
