
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "CInfoView.h"
#include "MainFrm.h"
using namespace Gdiplus;

void DrawLoadingMessage(Graphics& graphics, SolidBrush& brush, Gdiplus::Font& font);
void DrawModel(CWeatherSensorData& model, Graphics& graphics, SolidBrush& brush, Gdiplus::Font& font1, Gdiplus::Font& font2);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CChildView

CInfoView::CInfoView()
{
	paint_message_number = 0;
	pModel = NULL;
}

CInfoView::~CInfoView()
{
	if (pModel != NULL) {
		delete pModel;
	}
}

BEGIN_MESSAGE_MAP(CInfoView, CWnd)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(DATA_LOAD_COMPLETE, HandleDataLoadComplete)
END_MESSAGE_MAP()

// CChildView message handlers

// Async data loading from cloud has completed. We can draw stuff 
LRESULT CInfoView::HandleDataLoadComplete(WPARAM wParam, LPARAM lParam)
{
	CMFCApplication1App* pApp = (CMFCApplication1App*)lParam;
	if (pApp != NULL)
	{
		SensorRepository& sensors = pApp->GetSensors();
		CWeatherSensorData data = sensors.GetData();
		this->pModel = new CWeatherSensorData(data);
		TRACE("LOADED data timestamp %s\n", data.GetTimestamp());
		this->Invalidate();
		this->UpdateWindow();
	}
	return 0;
}



BOOL CInfoView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CInfoView::OnPaint() 
{
	TRACE(L"Painting %d\n", ++paint_message_number);

	CPaintDC dc(this); // device context for painting

	Graphics graphics(dc);
	SolidBrush brush(Color::Salmon);
	FontFamily fontFamily(L"Consolas");
	Gdiplus::Font font1(&fontFamily, 128, FontStyleRegular, UnitPixel);
	Gdiplus::Font font2(&fontFamily, 72, FontStyleRegular, UnitPixel);

	
	if (pModel == NULL) {
		DrawLoadingMessage(graphics, brush, font1);
	}
	else
	{
		DrawModel(*this->pModel, graphics, brush, font1, font2);
	}


	//Pen      pen(Color::Blue, 5.0f);
	//graphics.DrawLine(&pen, 0, 20, 200, 100);

}

void DrawLoadingMessage(Graphics& graphics, SolidBrush& brush, Gdiplus::Font& font) {
	PointF pointF(0.0f, 10.0f);
	graphics.DrawString(_T("Loading sensor data from the cloud...") , -1, &font, pointF, &brush);
}

void DrawModel(CWeatherSensorData& model, Graphics& graphics, SolidBrush& brush, Gdiplus::Font& font1, Gdiplus::Font& font2)
{
	USES_CONVERSION;

	double yLine = 10.0f;
	PointF pointLine1(0.0f, yLine);

	yLine += 150.0f;
	PointF pointLine2(0.0f, yLine);

	yLine += 150.0f;
	PointF pointLine3(0.0f, yLine);
	
	yLine += 150.0f;
	PointF pointLine4(0.0f, yLine);

	CString timeStamp = A2T(model.GetTimestamp());
	graphics.DrawString(timeStamp, -1, &font1, pointLine1, &brush);
	
	CString strPressure;
	strPressure.Format(_T("Pressure: %.3f inHG"), model.GetPressure());
	graphics.DrawString(strPressure, -1, &font1, pointLine2, &brush);

	CString strTemp;
	strPressure.Format(_T("Temperature: %.1f F"), model.GetTempInFarenheight());
	graphics.DrawString(strPressure, -1, &font1, pointLine3, &brush);
	
	graphics.DrawString(_T("Sensor: BMP180 connected to Raspberry Pi Zero"), -1, &font2, pointLine4, &brush);
}

void CInfoView::OnShowWindow(BOOL b, UINT ui)
{
	// TODO anything to do here??????
}

