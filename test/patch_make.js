var fs = require("fs");

var assert = require("assert");
var DMP = require("../build/Release/dmp");

// DMP.set_Patch_DeleteThreshold(0.25);
// DMP.set_Match_Threshold(0.25);
// DMP.set_Match_Distance(100);

var test1 = fs.readFileSync("test/test1");
var test2 = fs.readFileSync("test/test2");

var patches;
patches = DMP.patch_make(test1.toString(), test2.toString());

assert.notStrictEqual(patches.toString(), "", "Patches are empty for text that differs!");

patches = DMP.patch_make(test1, test2);

assert.notStrictEqual(patches.toString(), "", "Patches are empty for binaries that differ!");
