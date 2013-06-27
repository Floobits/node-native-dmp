var native_dmp = require("../build/Release/dmp");

var text1 = new Buffer("blah");
var text2 = new Buffer("blah123");
var patch = native_dmp.patch_make(text1, text2);

console.log(patch);
console.log(patch.toString());

var text3 = native_dmp.patch_apply(patch, text1);

console.log(text3);
console.log(text3.toString());
