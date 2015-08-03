# 4d-plugin-PDF2SVG
Convert PDF to SVG, using Poppler and Cairo. Inspired by https://github.com/db9052/pdf2svg

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

**Arguments**

* $1: The PDF document BLOB
* $2: ARRAY PICTURE to receive the pages (must be defined) //see remarks below
* $3: starting page (1 based). [optional]
* $4: ending page (1 based). [optional]
* $5: password [optional]
* $6: callback method [optional]

**Callback**

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

**Error codes**

```
#define PDF2SVG_ERROR_InvalidSourceData (-1)
#define PDF2SVG_ERROR_InvalidReturnType (-2)
#define PDF2SVG_ERROR_AbortedByUser (-3)
```

**Alternate syntax**

Alternatively, you may pass ```ARRAY TEXT``` or ```ARRAY BLOB``` in $2. However, $4 in the callback function will not be used. The current page data will only be passed to the callback method when ```ARRAY PICTURE``` is used. It's main purpose is to  display a preview image. Note that the image is a ref-counted duplicate of the final array element. 
