set(header
    ColorPicker.h
    FileBrowser.h
    FileBrowserDialog.h
    FileBrowserItemView.h
    FileBrowserPrivate.h
    FileBrowserSettings.h
	IOSettings.h
	IOSettingsWidget.h
    ISettingsWidget.h
    LanguageSettingsWidget.h
	LineGraphWidget.h
    ListViewHeader.h
    PPMSettingsWidget.h
    PaletteSettingsWidget.h
    Render2DSettingsWidget.h
    SearchBox.h
    SizeSettingsWidget.h
	ThermometerWidget.h
	TimeSettingsWidget.h
    TooltipsSettingsWidget.h
    UIComponentsSystem.h)
set(source
    ColorPicker.cpp
    FileBrowser.cpp
    FileBrowserDialog.cpp
    FileBrowserDrivesWidget.cpp
    FileBrowserItemView.cpp
    FileBrowserPathWidget.cpp
    FileBrowserRecentPathsWidget.cpp
    FileBrowserSettings.cpp
    FileBrowserShortcutsModel.cpp
    FileBrowserShortcutsWidget.cpp
	IOSettings.cpp
	IOSettingsWidget.cpp
    ISettingsWidget.cpp
    LanguageSettingsWidget.cpp
	LineGraphWidget.cpp
    ListViewHeader.cpp
    PPMSettingsWidget.cpp
    PaletteSettingsWidget.cpp
    Render2DSettingsWidget.cpp
    SearchBox.cpp
    SizeSettingsWidget.cpp
	ThermometerWidget.cpp
	TimeSettingsWidget.cpp
    TooltipsSettingsWidget.cpp
    UIComponentsSystem.cpp)

if(FFmpeg_FOUND)
    set(header
        ${header}
        FFmpegSettingsWidget.h)
    set(source
        ${source}
        FFmpegSettingsWidget.cpp)
endif()
if(JPEG_FOUND)
    set(header
        ${header}
        JPEGSettingsWidget.h)
    set(source
        ${source}
        JPEGSettingsWidget.cpp)
endif()
if(OPENEXR_FOUND)
    set(header
        ${header}
    	OpenEXRSettingsWidget.h)
    set(source
        ${source}
    	OpenEXRSettingsWidget.cpp)
endif()
if(TIFF_FOUND)
    set(header
        ${header}
        TIFFSettingsWidget.h)
    set(source
        ${source}
        TIFFSettingsWidget.cpp)
endif()

add_library(djvUIComponents ${header} ${source})
target_link_libraries(djvUIComponents djvUI)
set_target_properties(
    djvUIComponents
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)
