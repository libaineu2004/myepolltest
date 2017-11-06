TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

#DESTDIR = ../bin

SOURCES += main.cpp \
    net_tcp.cpp \
    net_epoller.cpp \
    net_udp.cpp \
    net_run.cpp \
    net_session.cpp \
    data_handle.cpp \
    ipc_pipe.cpp \
    ipc_sem.cpp \
    ipc_shm.cpp \
    modbus_data.cpp \
    ds_queue.cpp

HEADERS += \
    net_tcp.h \
    net_epoller.h \
    net_udp.h \
    net_run.h \
    net_session.h \
    data_handle.h \
    ipc_pipe.h \
    ipc_sem.h \
    ipc_shm.h \
    modbus_data.h \
    ds_queue.h
