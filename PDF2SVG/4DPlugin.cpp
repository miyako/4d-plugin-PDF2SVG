/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : PDF2SVG
 #	author : miyako
 #	2015/08/01
 #
 # --------------------------------------------------------------------------------*/


#include "4DPluginAPI.h"
#include "4DPlugin.h"

bool IsProcessOnExit(){    
    C_TEXT name;
    PA_long32 state, time;
    PA_GetProcessInfo(PA_GetCurrentProcessNumber(), name, &state, &time);
    CUTF16String procName(name.getUTF16StringPtr());
    CUTF16String exitProcName((PA_Unichar *)"$\0x\0x\0\0\0");
    return (!procName.compare(exitProcName));
}

void OnStartup(){  

    //define environment varaibles: [FONTCONFIG_FILE, FONTCONFIG_PATH]

#if VERSIONMAC
    NSBundle *thisBundle = [NSBundle bundleWithIdentifier:@"com.4D.4DPlugin.PDF2SVG"];
    if(thisBundle){
        NSString *FONTCONFIG_FILE = [thisBundle pathForResource:@"fonts" ofType:@"conf" inDirectory:@"fonts"];
        if(FONTCONFIG_FILE){
            NSString *FONTCONFIG_PATH = [FONTCONFIG_FILE stringByDeletingLastPathComponent];
            //but we will still see lots of "some font thing failed"
            setenv("FONTCONFIG_FILE", [FONTCONFIG_FILE UTF8String], 0);
            setenv("FONTCONFIG_PATH", [FONTCONFIG_PATH UTF8String], 0);   
        }
    }
#else
    wchar_t	thisPath[_MAX_PATH] = {0};
	wchar_t	fDrive[_MAX_DRIVE], fDir[_MAX_DIR], fName[_MAX_FNAME], fExt[_MAX_EXT];
    
    HMODULE hplugin = GetModuleHandleW(L"PDF2SVG.4DX");
    GetModuleFileNameW(hplugin, thisPath, _MAX_PATH);
    _wsplitpath_s(thisPath, fDrive, fDir, fName, fExt);
    
    std::wstring path = fDrive;
    path += fDir;//path to plugin parent folder
    
    if(path.at(path.size() - 1) == L'\\')//remove delimiter
        path = path.substr(0, path.size() - 1);
    
    _wsplitpath_s(path.c_str(), fDrive, fDir, fName, fExt);
    wchar_t resourcesPath[_MAX_PATH] = {0};
    _wmakepath_s(resourcesPath, fDrive, fDir, L"Resources\\", NULL);
    
    std::wstring FONTCONFIG_FILE = resourcesPath;
	FONTCONFIG_FILE += L"fonts\\fonts.conf";
    std::wstring FONTCONFIG_PATH = resourcesPath;
	FONTCONFIG_PATH += L"fonts\\";
    
    SetEnvironmentVariable(L"FONTCONFIG_FILE", FONTCONFIG_FILE.c_str());
    SetEnvironmentVariable(L"FONTCONFIG_PATH", FONTCONFIG_PATH.c_str());
#endif

#if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init();// Initialise the GType library
    //http://askubuntu.com/questions/262574/how-to-correct-glib-2-35-x-g-type-init-is-deprecated-error-when-compiling-from
#endif    
}

void OnCloseProcess(){  
    if(IsProcessOnExit()){
        //ceanup code here;  
    }
}

void PluginMain(PA_long32 selector, PA_PluginParameters params)
{
	try
	{
		PA_long32 pProcNum = selector;
		sLONG_PTR *pResult = (sLONG_PTR *)params->fResult;
		PackagePtr pParams = (PackagePtr)params->fParameters;

		CommandDispatcher(pProcNum, pResult, pParams); 
	}
	catch(...)
	{

	}
}

void CommandDispatcher (PA_long32 pProcNum, sLONG_PTR *pResult, PackagePtr pParams)
{
	switch(pProcNum)
	{
        case kInitPlugin :
        case kServerInitPlugin :            
            OnStartup();
            break;    

        case kCloseProcess :            
            OnCloseProcess();
            break;      
// --- Convert

		case 1 :
			PDF_Convert(pResult, pParams);
			break;

		case 2 :
			PDF_Get_page_count(pResult, pParams);
			break;

	}
}

// ------------------------------------ Convert -----------------------------------

static cairo_status_t
__cairo_write_func (void *closure, const unsigned char *data, unsigned int length)
{
    C_BLOB *ptr = (C_BLOB *)closure;
    ptr->addBytes((const uint8_t *)data, length);
    return CAIRO_STATUS_SUCCESS;
}

typedef enum{
 PDF2SVG_ReturnNothing = 0, 
 PDF2SVG_ReturnPicture = 1,   
 PDF2SVG_ReturnText = 2,
 PDF2SVG_ReturnBlob = 3 
}PDF2SVG_ReturnType;

#define PDF2SVG_ERROR_InvalidSourceData (-1)
#define PDF2SVG_ERROR_InvalidReturnType (-2)
#define PDF2SVG_ERROR_AbortedByUser (-3)

void PDF_Convert(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_LONGINT Param3;
	C_LONGINT Param4;
	C_TEXT Param5;
	C_TEXT Param6;
//	C_TEXT Param7;
	C_LONGINT returnValue;

	Param1.fromParamAtIndex(pParams, 1);
	Param3.fromParamAtIndex(pParams, 3);
	Param4.fromParamAtIndex(pParams, 4);
	Param5.fromParamAtIndex(pParams, 5);
	Param6.fromParamAtIndex(pParams, 6);
//	Param7.fromParamAtIndex(pParams, 7);

    PA_Variable Param2 = *((PA_Variable*) pParams[1]);
    
    switch (Param2.fType) 
	{
		case eVK_ArrayPicture:
			break;
		case eVK_ArrayUnicode:
			break;            
		case eVK_Undefined:
			PA_ClearVariable(&Param2);				
			Param2 = PA_CreateVariable(eVK_ArrayPicture);
			break;				
		default:
			break;
	}

    PDF2SVG_ReturnType returnType = PDF2SVG_ReturnNothing;

    switch (Param2.fType) 
	{
        case eVK_ArrayPicture:
            PA_ResizeArray(&Param2, 0);
            returnType = PDF2SVG_ReturnPicture;
			break;
		case eVK_ArrayUnicode:
            PA_ResizeArray(&Param2, 0);
            returnType = PDF2SVG_ReturnText;
			break;
#ifdef eVK_ArrayBlob             
		case eVK_ArrayBlob:
            PA_ResizeArray(&Param2, 0);
            returnType = PDF2SVG_ReturnBlob;
			break;  
#endif                       
    }

    int startPage = Param3.getIntValue();
    int finalPage = Param4.getIntValue();

    if(returnType != PDF2SVG_ReturnNothing){

        //callback
        method_id_t methodId = PA_GetMethodID((PA_Unichar *)Param6.getUTF16StringPtr());
        bool abortedByCallbackMethod = false;
        bool isCallbackActive = false;
        int number_entry;
         
        PopplerDocument *pdffile;
        PopplerPage *page;
        
        char *data = (char *)Param1.getBytesPtr();
        int length = (int)Param1.getBytesLength();
        
        CUTF8String Password;
        Param5.copyUTF8String(&Password);
        const char *password = Password.length() ? (const char *)Password.c_str() : NULL;
        
        pdffile = poppler_document_new_from_data(data, length, password, NULL);
        
        if(pdffile){
    
            int pageCount = poppler_document_get_n_pages(pdffile);
            
            startPage = (startPage > 0) ? startPage : 1;
            startPage = (startPage < pageCount) ? startPage : pageCount;
            
            finalPage = (finalPage > startPage) ? finalPage : pageCount;
            finalPage = (finalPage < pageCount) ? finalPage : pageCount;
            
            startPage--;
            finalPage--;
            
            PA_ResizeArray(&Param2, pageCount);// we will start from array element #1
            
            if(methodId){
                number_entry = (finalPage - startPage) + 1;
                isCallbackActive = true;
            }
            
            int currentpageInd = 0;
            
            for(int pageInd = startPage; pageInd < pageCount; ++pageInd) {
                                
                currentpageInd++;
                
                PA_YieldAbsolute();
                
                page = poppler_document_get_page(pdffile, pageInd);
                
                if(page){
                
                    C_BLOB ImageData;
                    double width, height;
                    poppler_page_get_size (page, &width, &height);
                    cairo_surface_t *surface = cairo_svg_surface_create_for_stream(__cairo_write_func, &ImageData, width, height);
										cairo_svg_surface_restrict_to_version(surface, CAIRO_SVG_VERSION_1_1);
										cairo_t *drawcontext = cairo_create(surface);
                    poppler_page_render_for_printing(page, drawcontext);
                    cairo_show_page(drawcontext);
                    cairo_destroy(drawcontext);
                    cairo_surface_destroy(surface);
                    g_object_unref(page);
                    
                    if(ImageData.getBytesLength()){
                       
                        switch (Param2.fType) 
                        {
                            case eVK_ArrayPicture:
                            
                                PA_Picture picture;
                                picture = PA_CreatePicture((void *)ImageData.getBytesPtr(), ImageData.getBytesLength());	
                                PA_SetPictureInArray(Param2, pageInd + 1, picture);
                                
                                //callback
                                if(isCallbackActive){
                                    PA_Variable	params[4];
                                    params[0] = PA_CreateVariable(eVK_Longint);
                                    params[1] = PA_CreateVariable(eVK_Longint);
                                    params[2] = PA_CreateVariable(eVK_Longint);
                                    params[3] = PA_CreateVariable(eVK_Picture);

                                    PA_SetLongintVariable(&params[0], currentpageInd);
                                    PA_SetLongintVariable(&params[1], number_entry);
                                    PA_SetLongintVariable(&params[2], pageInd + 1);
                                    
                                    PA_Picture copyPicture = PA_DuplicatePicture(picture, 1);
                                    PA_SetPictureVariable(&params[3], copyPicture);
                                    PA_Variable result = PA_ExecuteMethodByID(methodId, params, 4);
                                    PA_DisposePicture(copyPicture);
                                    if(PA_GetVariableKind(result) == eVK_Boolean){
                                        abortedByCallbackMethod = PA_GetBooleanVariable(result);
                                    }
                                    
                                }
                                break;   
                                
                            case eVK_ArrayUnicode:
                            
                                C_TEXT svg;
                                svg.setUTF8String((const uint8_t *)ImageData.getBytesPtr(), ImageData.getBytesLength());
                                PA_Unistring u = PA_CreateUnistring((PA_Unichar *)svg.getUTF16StringPtr());
                                PA_SetStringInArray(Param2, pageInd + 1, &u);
                                
                                //callback
                                if(isCallbackActive){
                                    PA_Variable	params[3];
                                    params[0] = PA_CreateVariable(eVK_Longint);
                                    params[1] = PA_CreateVariable(eVK_Longint);
                                    params[2] = PA_CreateVariable(eVK_Longint);

                                    PA_SetLongintVariable(&params[0], currentpageInd);
                                    PA_SetLongintVariable(&params[1], number_entry);
                                    PA_SetLongintVariable(&params[2], pageInd + 1);
                                    
                                    PA_Variable result = PA_ExecuteMethodByID(methodId, params, 4);
                                    if(PA_GetVariableKind(result) == eVK_Boolean){
                                        abortedByCallbackMethod = PA_GetBooleanVariable(result);
                                    }
                                    
                                }
                                break;
#ifdef eVK_ArrayBlob                                             
                            case eVK_ArrayBlob:
                                PA_Blob blob;
                                blob.fSize = ImageData.getBytesLength();
                                blob.fHandle = PA_NewHandle(blob.fSize);
                                PA_Handle h1 = PA_LockHandle(ImageData.getBytesPtr()); 
                                PA_Handle h2 = PA_LockHandle(blob.fHandle); 
                                PA_MoveBlock(h1, h2, blob.fSize);
                                PA_UnlockHandle(h1);
                                PA_UnlockHandle(h2);
                                PA_SetBlobInArray(Param2, pageInd + 1, blob);
 
                                //callback
                                if(isCallbackActive){
                                    PA_Variable	params[3];
                                    params[0] = PA_CreateVariable(eVK_Longint);
                                    params[1] = PA_CreateVariable(eVK_Longint);
                                    params[2] = PA_CreateVariable(eVK_Longint);

                                    PA_SetLongintVariable(&params[0], currentpageInd);
                                    PA_SetLongintVariable(&params[1], number_entry);
                                    PA_SetLongintVariable(&params[2], pageInd + 1);

                                    PA_Variable result = PA_ExecuteMethodByID(methodId, params, 4);
                                    if(PA_GetVariableKind(result) == eVK_Boolean){
                                        abortedByCallbackMethod = PA_GetBooleanVariable(result);
                                    }
                                    
                                }
                                break;  
#endif                                                             
                        }
                      
                    }//ImageData.getBytesLength()

                }//page
                
                if(abortedByCallbackMethod){
                    pageInd = pageCount;
                    returnValue.setIntValue(PDF2SVG_ERROR_AbortedByUser);   
                }

            }
    
            g_object_unref(pdffile);
            
        }else{
            returnValue.setIntValue(PDF2SVG_ERROR_InvalidSourceData);
        }
        
        PA_Variable *param = ((PA_Variable *)pParams[1]);
        param->fType = Param2.fType;
        param->fFiller = Param2.fFiller;
        param->uValue.fArray.fCurrent = Param2.uValue.fArray.fCurrent;
        param->uValue.fArray.fNbElements = Param2.uValue.fArray.fNbElements;
        param->uValue.fArray.fData = Param2.uValue.fArray.fData;    
        
    }else{
        returnValue.setIntValue(PDF2SVG_ERROR_InvalidReturnType);
    }

	returnValue.setReturn(pResult);
}

void PDF_Get_page_count(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_TEXT Param2;
//	C_TEXT Param3;
	C_LONGINT returnValue;

	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
//	Param3.fromParamAtIndex(pParams, 3);

	PopplerDocument *pdffile;
        
    char *data = (char *)Param1.getBytesPtr();
    int length = (int)Param1.getBytesLength();

    CUTF8String Password;
    Param2.copyUTF8String(&Password);
    const char *password = Password.length() ? (const char *)Password.c_str() : NULL;
    
    pdffile = poppler_document_new_from_data(data, length, password, NULL);

    if(pdffile){
        returnValue.setIntValue(poppler_document_get_n_pages(pdffile));
    }else{
        returnValue.setIntValue(PDF2SVG_ERROR_InvalidSourceData);
    }

	returnValue.setReturn(pResult);
}
