# 4d-plugin-PDF2SVG
Convert PDF to SVG.

Powered by... 

libcairo
libffi
libfontconfig
libfreetype
libgio
libglib
libgmodule
libgobject
libharfbuzz
libiconv
libintl
libjpeg
libpixman
libpng16
libpoppler
libtiff
libz

Size is large (``105.7 MB``).

**Related Project**: https://github.com/miyako/4d-plugin-svg-converter-light

**Related Project**: https://github.com/miyako/4d-plugin-svg-converter

## Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|

### Version

<img src="https://cloud.githubusercontent.com/assets/1725068/18940649/21945000-8645-11e6-86ed-4a0f800e5a73.png" width="32" height="32" /> <img src="https://cloud.githubusercontent.com/assets/1725068/18940648/2192ddba-8645-11e6-864d-6d5692d55717.png" width="32" height="32" />

![](https://github.com/miyako/4d-plugin-PDF2SVG/blob/master/images/screenshot.png)

![](https://github.com/miyako/4d-plugin-PDF2SVG/blob/master/images/screenshot-w.png)

```
error:=PDF Convert (pdf;svg;from;to;password;method)
```

Parameter|Type|Description
------------|------|----
pdf|BLOB|The PDF document BLOB
svg|ARRAY PICTURE (or ARRAY TEXT or ARRAY BLOB)|Array to receive the pages (must be defined); See remarks below
from|INT32|Starting page (1 based) [optional]
to|INT32|Ending page (1 based) [optional]
password|TEXT|Password [optional]
method|TEXT|Callback method [optional]
error|INT32|Error code

```
count:=PDF Get page count (pdf;password)
```

Parameter|Type|Description
------------|------|----
pdf|BLOB|The PDF document BLOB
password|TEXT|Password [optional]
count|INT32|Page count

### Examples

```
$path:=Get 4D folder(Current resources folder)+"sample.pdf"
DOCUMENT TO BLOB($path;$PDF)

ARRAY PICTURE($pages;0)
$startPage:=0
$endPage:=0
$password:=""
$callback:="PDF2SVG_CB_PICTURE"

<>p:=Progress New 
Progress SET PROGRESS (<>P;0)
Progress SET BUTTON ENABLED (<>P;True)

$error:=PDF Convert ($PDF;$pages;$startPage;$endPage;$password;$callback)

Progress QUIT (<>P)
```

### Callback

The method is executed in the same process as the calling method.

Parameters:

```
C_LONGINT($1;$pos)//1-based
C_LONGINT($2;$total)
C_LONGINT($3;$pageNumber)//1-based
C_PICTURE($4;$page)
C_BOOLEAN($0;$stop)
```

To display a progress bar:

```
Progress SET PROGRESS (<>P;$pos/$total)
```

To abort:

```
$0:=Progress Stopped (<>P)
```

### Error codes

```
#define PDF2SVG_ERROR_InvalidSourceData (-1)
#define PDF2SVG_ERROR_InvalidReturnType (-2)
#define PDF2SVG_ERROR_AbortedByUser (-3)
```

### Alternate syntax

Alternatively, you may pass ```ARRAY TEXT``` or ```ARRAY BLOB``` in $2. However, $4 in the callback function will not be used. The current page data will only be passed to the callback method when ```ARRAY PICTURE``` is used. It's main purpose is to  display a preview image. Note that the image is a ref-counted duplicate of the final array element. 
