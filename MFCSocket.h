#pragma once
#include <afxsock.h>

// 自定义 Socket 消息
#define WM_SOCKETACCEPT  (WM_USER + 100)   // 接收连接请求
#define WM_SOCKETCLOSE   (WM_USER + 101)   // 连接关闭
#define WM_SOCKETCONNECT (WM_USER + 102)   // 连接建立
#define WM_SOCKETRECEIVE (WM_USER + 103)   // 接收数据
#define WM_SOCKETSEND    (WM_USER + 104)   // 发送数据
#define WM_SOCKETOOBDATA (WM_USER + 105)   // 接收带外数据 TCP 套接字中的带外数据（Out-of-Band Data, OOB）


/*
* ON_MESSAGE(WM_SOCKETACCEPT, &函数)  afx_msg LRESULT AcceptClient(WPARAM wParam, LPARAM lParam);
* ON_MESSAGE(WM_SOCKETCLOSE, &函数)   afx_msg LRESULT SocketClose(WPARAM wParam, LPARAM lParam);
* ON_MESSAGE(WM_SOCKETCONNECT, &函数) afx_msg LRESULT SocketConnect(WPARAM wParam, LPARAM lParam);
* ON_MESSAGE(WM_SOCKETRECEIVE, &函数) afx_msg LRESULT SocketReceive(WPARAM wParam, LPARAM lParam);
* ON_MESSAGE(WM_SOCKETSEND, &函数)    afx_msg LRESULT SocketSend(WPARAM wParam, LPARAM lParam);
* ON_MESSAGE(WM_SOCKETOOBDATA, &函数) afx_msg LRESULT SocketOOBData(WPARAM wParam, LPARAM lParam);
*/
class Socket : public CAsyncSocket
{
private:
    HWND hNotifyDialog = NULL;  // 指向关联对话框的句柄
public:
    Socket(HWND NotifyDialog = NULL) {
		hNotifyDialog = NotifyDialog;
    };
    ~Socket() {
        Close();
        hNotifyDialog = NULL;
    }

    bool SetNotifyDialog(HWND NotifyDialog)    {
		hNotifyDialog = NotifyDialog;
        return hNotifyDialog != NULL;
	}

    // 创建服务器 socket
    bool CreateServer(int nPort){
        // IP 为空表示 INADDR_ANY
        if (!Create(nPort, SOCK_STREAM, FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE)) return false;

        // 关键！必须调用 Listen() 才会触发 OnAccept
        if (!Listen()){
            DWORD err = GetLastError();
            TRACE(_T("Listen failed: %d\n"), err);
            Close();
            return false;
        }
        return true;
    }

    // 创建客户端 socket 并连接
    bool CreateClient(LPCTSTR strIP, int nPort){
        if (!Create(0, SOCK_STREAM, FD_CONNECT | FD_CLOSE | FD_READ | FD_WRITE)) return false;
        return !Connect(strIP, nPort);
    }

    // 创建 UDP 服务器/端点
    bool CreateUDP(int nPort){
        return !Create(nPort, SOCK_DGRAM, FD_READ);
    }

protected:
    // Socket 消息处理程序 (类似于VB6.0中的事件)
    virtual void OnAccept(int nErrorCode) {
        if (!nErrorCode && hNotifyDialog) {PostMessage(hNotifyDialog, WM_SOCKETACCEPT, (WPARAM)this, (LPARAM)nErrorCode);}
		CAsyncSocket::OnAccept(nErrorCode);
    };
    virtual void OnClose(int nErrorCode) {
        if (!nErrorCode && hNotifyDialog) {PostMessage(hNotifyDialog, WM_SOCKETCLOSE, (WPARAM)this, (LPARAM)nErrorCode);}
		CAsyncSocket::OnClose(nErrorCode);
    };
    virtual void OnConnect(int nErrorCode) {
        if (!nErrorCode && hNotifyDialog) {PostMessage(hNotifyDialog, WM_SOCKETCONNECT, (WPARAM)this, (LPARAM)nErrorCode);}
		CAsyncSocket::OnConnect(nErrorCode);
    };
    virtual void OnOutOfBandData(int nErrorCode) {
        if (!nErrorCode && hNotifyDialog) {PostMessage(hNotifyDialog, WM_SOCKETOOBDATA, (WPARAM)this, (LPARAM)nErrorCode);}
		CAsyncSocket::OnOutOfBandData(nErrorCode);
    };
    virtual void OnReceive(int nErrorCode) {
        if (!nErrorCode && hNotifyDialog) {PostMessage(hNotifyDialog, WM_SOCKETRECEIVE, (WPARAM)this, (LPARAM)nErrorCode);}
		CAsyncSocket::OnReceive(nErrorCode);
    };
    virtual void OnSend(int nErrorCode) {
        if (!nErrorCode && hNotifyDialog) {PostMessage(hNotifyDialog, WM_SOCKETSEND, (WPARAM)this, (LPARAM)nErrorCode);}
		CAsyncSocket::OnSend(nErrorCode);
    };
};

