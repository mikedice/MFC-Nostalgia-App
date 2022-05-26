
// MFCApplication1.h : main header file for the MFCApplication1 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "SensorRepository.h"

// This class controls the running of the application
class CMFCApplication1App : public CWinApp
{
public:
	CMFCApplication1App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void LoadSensorRepository();
	SensorRepository& GetSensors();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	SensorRepository sensorRepository;
public:
	afx_msg void OnViewRefresh();
};

extern CMFCApplication1App theApp;
