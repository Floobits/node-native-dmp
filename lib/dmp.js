var native_dmp = require("../build/Release/dmp");

var text1 = new Buffer("blah");
var text2 = new Buffer("blah123");
var patch = native_dmp.patch_make(text1, text2);

console.log("text1:", text1);
console.log("text2:", text2);
console.log("patch:");
console.log(patch);


var text3 = native_dmp.patch_make(new Buffer([1,2,3,0,4]), new Buffer('asdff'));

console.log("result", text3);

var b1 = new Buffer([0, 1, 2, 145]);
// var b1 = new Buffer("asdfe");
var b2 = new Buffer("asdf");

var patch = native_dmp.patch_make(b1, b2);

console.log(b1, b2, "patch:", patch);
