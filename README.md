# Native Diff-Match-Patch

C++ version of [diff-match-patch](https://code.google.com/p/google-diff-match-patch/) exposed as a node module. This uses a heavily-modified version of the C++ diff-match-patch. **It is binary safe.**

### Development status: Working. We use it in production.

[![Floobits Status](https://floobits.com/Floobits/native-dmp.svg)](https://floobits.com/Floobits/native-dmp/redirect)

## Usage

Here is an example of applying a patch:

    var buffer,
      DMP,
      result,
      patch_text;

    DMP = require("native-diff-match-patch");

    // If you like, you can change DMP's thresholds
    DMP.set_Patch_DeleteThreshold(0.375);
    DMP.set_Match_Threshold(0.375);
    DMP.set_Match_Distance(100);

    buffer = "abc";
    patch_text = "@@ -1,3 +1,2 @@\n" +
                 " ab\n" +
                 "-c";

    result = DMP.patch_apply(patch_text, buffer);

    for (i = 0; i < result[1].length; i++) {
      if (result[1][i] !== true) {
        throw new Error("Patch not applied cleanly.");
      }
    }

    buffer = result[0];

    // This prints "ab"
    console.log(buffer);
