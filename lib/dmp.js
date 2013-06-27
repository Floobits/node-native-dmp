var native_dmp = require("../build/Release/dmp");

var text1 = new Buffer("blah");
var text2 = new Buffer("blah123");
var patch = native_dmp.patch_make(text1, text2);

console.log(patch, patch.toString());
