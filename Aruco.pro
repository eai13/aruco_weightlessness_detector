QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia core gui testlib

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    framerate.cpp \
    main.cpp \
    mainwindow.cpp \
    memfile.cpp \
    settings.cpp

HEADERS += \
    config.h \
    framerate.h \
    mainwindow.h \
    memfile.h \
    settings.h \
    ui_upd.h

FORMS += \
    mainwindow.ui \
    settings.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../opencv/x64/vc16/lib/ -lopencv_aruco452
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../opencv/x64/vc16/lib/ -lopencv_aruco452d

#INCLUDEPATH += $$PWD/../opencv/include
#DEPENDPATH += $$PWD/../opencv/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../opencv/x64/vc16/lib/ \
-lopencv_aruco452 \
-lopencv_aruco452 \
-lopencv_bgsegm452 \
-lopencv_bioinspired452 \
-lopencv_calib3d452 \
-lopencv_ccalib452 \
-lopencv_core452 \
-lopencv_datasets452 \
-lopencv_dnn452 \
-lopencv_dnn_objdetect452 \
-lopencv_dnn_superres452 \
-lopencv_dpm452 \
-lopencv_face452 \
-lopencv_features2d452 \
-lopencv_flann452 \
-lopencv_fuzzy452 \
-lopencv_gapi452 \
-lopencv_hfs452 \
-lopencv_highgui452 \
-lopencv_imgcodecs452 \
-lopencv_imgproc452 \
-lopencv_img_hash452 \
-lopencv_intensity_transform452 \
-lopencv_line_descriptor452 \
-lopencv_mcc452 \
-lopencv_ml452 \
-lopencv_objdetect452 \
-lopencv_optflow452 \
-lopencv_phase_unwrapping452 \
-lopencv_photo452 \
-lopencv_plot452 \
-lopencv_quality452 \
-lopencv_rapid452 \
-lopencv_reg452 \
-lopencv_rgbd452 \
-lopencv_saliency452 \
-lopencv_shape452 \
-lopencv_stereo452 \
-lopencv_stitching452 \
-lopencv_structured_light452 \
-lopencv_superres452 \
-lopencv_surface_matching452 \
-lopencv_text452 \
-lopencv_tracking452 \
-lopencv_video452 \
-lopencv_videoio452 \
-lopencv_videostab452 \
-lopencv_wechat_qrcode452 \
-lopencv_xfeatures2d452 \
-lopencv_ximgproc452 \
-lopencv_xobjdetect452 \
-lopencv_xphoto452
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../opencv/x64/vc16/lib/ \
-lopencv_aruco452d \
-lopencv_aruco452d \
-lopencv_bgsegm452d \
-lopencv_bioinspired452d \
-lopencv_calib3d452d \
-lopencv_ccalib452d \
-lopencv_core452d \
-lopencv_datasets452d \
-lopencv_dnn452d \
-lopencv_dnn_objdetect452d \
-lopencv_dnn_superres452d \
-lopencv_dpm452d \
-lopencv_face452d \
-lopencv_features2d452d \
-lopencv_flann452d \
-lopencv_fuzzy452d \
-lopencv_gapi452d \
-lopencv_hfs452d \
-lopencv_highgui452d \
-lopencv_imgcodecs452d \
-lopencv_imgproc452d \
-lopencv_img_hash452d \
-lopencv_intensity_transform452d \
-lopencv_line_descriptor452d \
-lopencv_mcc452d \
-lopencv_ml452d \
-lopencv_objdetect452d \
-lopencv_optflow452d \
-lopencv_phase_unwrapping452d \
-lopencv_photo452d \
-lopencv_plot452d \
-lopencv_quality452d \
-lopencv_rapid452d \
-lopencv_reg452d \
-lopencv_rgbd452d \
-lopencv_saliency452d \
-lopencv_shape452d \
-lopencv_stereo452d \
-lopencv_stitching452d \
-lopencv_structured_light452d \
-lopencv_superres452d \
-lopencv_surface_matching452d \
-lopencv_text452d \
-lopencv_tracking452d \
-lopencv_video452d \
-lopencv_videoio452d \
-lopencv_videostab452d \
-lopencv_wechat_qrcode452d \
-lopencv_xfeatures2d452d \
-lopencv_ximgproc452d \
-lopencv_xobjdetect452d \
-lopencv_xphoto452d

INCLUDEPATH += $$PWD/../opencv/include
DEPENDPATH += $$PWD/../opencv/include
