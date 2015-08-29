"use strict";
const fs = require("fs");

const assert = require("assert");
const DMP = require("../build/Release/dmp");

// DMP.set_Patch_DeleteThreshold(0.25);
// DMP.set_Match_Threshold(0.25);
// DMP.set_Match_Distance(100);

/*eslint-disable no-sync */
const test1 = fs.readFileSync("test/test1");
const test2 = fs.readFileSync("test/test2");
/*eslint-enable no-sync */

let patches = DMP.patch_make(test1.toString(), test2.toString());

assert.notStrictEqual(patches.toString(), "", "Patches are empty for text that differs!");

patches = DMP.patch_make(test1, test2);

assert.notStrictEqual(patches.toString(), "", "Patches are empty for binaries that differ!");
