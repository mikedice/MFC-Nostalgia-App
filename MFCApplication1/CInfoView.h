
// ChildView.h : interface of the CChildView class
//


#pragma once


// CChildView window
#include "CWeatherSensorData.h"

class CInfoView : public CWnd
{
// Construction
public:
	CInfoView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CInfoView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL b, UINT ui);
	afx_msg LRESULT HandleDataLoadComplete(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	int paint_message_number;
	CWeatherSensorData* pModel;

};

