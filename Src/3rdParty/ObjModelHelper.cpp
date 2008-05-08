#include "ObjModelHelper.h"

#include "WWhizInterface3.h"

extern WWhizInterface* g_wwhizInterface;

#ifdef WWHIZ_VC6

CString ObjModelHelper::GetCurrentConfigName()
{
	CComPtr<IDispatch> pDispConfig;
	g_pApplication->get_ActiveConfiguration(&pDispConfig);
	if (!pDispConfig)
	{
		return "Win32 (x86)";
	}

	CComBSTR bstrConfig;
	CComQIPtr<IConfigurationVC, &IID_IConfigurationVC> pConfigVC;
//	CComQIPtr<IConfigurationEVC, &IID_IConfigurationEVC> pConfigEVC;
	if (m_pDocumentsVC)
	{
		pConfigVC = pDispConfig;
		if (pConfigVC)
			pConfigVC->get_Name(&bstrConfig);

	}
//	else if (m_pDocumentsEVC)
//	{
//		pConfigEVC = pDispConfig;
//		if (pConfigEVC)
//			pConfigEVC->get_Name(&bstrConfig);
//	}
	
	CString configName = bstrConfig;
	int dashPos = configName.Find('-');
	if (dashPos != -1)
	{
		configName = configName.Mid(dashPos + 1);
		configName.TrimLeft();
		int spacePos = configName.ReverseFind(' ');
		if (spacePos != -1)
		{
			configName = configName.Left(spacePos);
			if (configName == "Win32")
			{
				configName += " (x86)";
			}
		}
	}
	
	return configName;
}


bool ObjModelHelper::SaveAll()
{
//    CComPtr<IUnknown> pUnknown;
//    CComPtr<IDispatch> pDispatch;
//    CComQIPtr<IEnumVARIANT, &IID_IEnumVARIANT> pNewEnum;

	if (m_pDocumentsVC)
	{
		// Get the number of projects in the collection
		long documentCount;
		m_pDocumentsVC->get_Count(&documentCount);

		// Iterate all the projects.
		for (long i = 1; i <= documentCount; i++)
		{
			// Get the next document
			CComPtr<IDispatch> pDispDocument;
			m_pDocumentsVC->Item(CComVariant(i), &pDispDocument);
			CComQIPtr<IGenericDocumentVC, &IID_IGenericDocumentVC> pGenericDocument(pDispDocument);
			if (!pGenericDocument)
				continue;

			short saved;
			if (SUCCEEDED(pGenericDocument->get_Saved(&saved)))
			{
				if (saved == VARIANT_FALSE)
				{
					DsSaveStatus saveStatus;
					if (SUCCEEDED(pGenericDocument->Save(CComVariant(), CComVariant(VARIANT_FALSE), &saveStatus)))
					{
						CComBSTR fullName;
						if (SUCCEEDED(pGenericDocument->get_FullName(&fullName)))
						{
							g_wwhizInterface->AddChangedFile(CString(fullName));
						}
					}
				}
			}
		}
	}
	else if (m_pDocumentsEVC)
	{
		// Get the number of projects in the collection
		long documentCount;
		m_pDocumentsEVC->get_Count(&documentCount);

		// Iterate all the projects.
		for (long i = 1; i <= documentCount; i++)
		{
			// Get the next document
			CComPtr<IDispatch> pDispDocument;
			m_pDocumentsEVC->Item(CComVariant(i), &pDispDocument);
			CComQIPtr<IGenericDocumentEVC, &IID_IGenericDocumentEVC> pGenericDocument(pDispDocument);
			if (!pGenericDocument)
				continue;

			short saved;
			if (SUCCEEDED(pGenericDocument->get_Saved(&saved)))
			{
				if (saved == VARIANT_FALSE)
				{
					DsSaveStatus saveStatus;
					if (SUCCEEDED(pGenericDocument->Save(CComVariant(), CComVariant(VARIANT_FALSE), &saveStatus)))
					{
						CComBSTR fullName;
						if (SUCCEEDED(pGenericDocument->get_FullName(&fullName)))
						{
							g_wwhizInterface->AddChangedFile(CString(fullName));
						}
					}
				}
			}
		}
	}
	
	return true;
}


bool ObjModelHelper::SetKeyBinding(bool justText, const CString& command, const CString& binding)
{
	if (!justText)
	{
		g_pApplication->AddKeyBinding(CComBSTR(binding), CComBSTR(command), CComBSTR("Main"));
	}
	g_pApplication->AddKeyBinding(CComBSTR(binding), CComBSTR(command), CComBSTR("Text"));
	return true;
}


#endif WWHIZ_VC6


//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

#ifdef WWHIZ_VSNET

CString ObjModelHelper::GetCurrentConfigName()
{
	CComPtr<EnvDTE::_Solution> pSolution;
	CComPtr<EnvDTE::SolutionBuild> pSolutionBuild;
	CComPtr<EnvDTE::SolutionConfiguration> pSolutionConfiguration;
	g_pDTE->get_Solution(&pSolution);
	if (!pSolution)
	{
		return "Win32 (x86)";
	}

	pSolution->get_SolutionBuild(&pSolutionBuild);
	if (!pSolutionBuild)
	{
		return "Win32 (x86)";
	}

	CComBSTR bstrConfig;

	pSolutionBuild->get_ActiveConfiguration(&pSolutionConfiguration);
	if (pSolutionConfiguration)
	{
		pSolutionConfiguration->get_Name(&bstrConfig);
	}
	
	CString configName(bstrConfig);
	int dashPos = configName.Find('-');
	if (dashPos != -1)
	{
		configName = configName.Mid(dashPos + 1);
		configName.TrimLeft();
		int spacePos = configName.ReverseFind(' ');
		if (spacePos != -1)
		{
			configName = configName.Left(spacePos);
			if (configName == "Win32")
			{
				configName += " (x86)";
			}
		}
	}
	
	return configName;
}


bool ObjModelHelper::SaveAll()
{
	CComPtr<EnvDTE::Documents> pDocuments;
	g_pDTE->get_Documents(&pDocuments);

	if (pDocuments)
	{
		// Get the number of projects in the collection
		long documentCount;
		pDocuments->get_Count(&documentCount);

		// Iterate all the projects.
		for (long i = 1; i <= documentCount; i++)
		{
			// Get the next document
			CComPtr<EnvDTE::Document> pDocument;
			pDocuments->Item(CComVariant(i), &pDocument);
			if (!pDocument)
				continue;

			short saved;
			if (SUCCEEDED(pDocument->get_Saved(&saved)))
			{
				if (saved == VARIANT_FALSE)
				{
					EnvDTE::vsSaveStatus saveStatus;
					if (SUCCEEDED(pDocument->Save(CComBSTR(), &saveStatus)))
					{
						CComBSTR fullName;
						if (SUCCEEDED(pDocument->get_FullName(&fullName)))
						{
							g_wwhizInterface->AddChangedFile(CString(fullName));
						}
					}
				}
			}
		}
	}
	
	return true;
}


bool ObjModelHelper::SetKeyBinding(bool justText, const CString& command, const CString& binding)
{
	CComPtr<EnvDTE::Commands> pCommands;
	g_pDTE->get_Commands(&pCommands);

	CComPtr<EnvDTE::Command> pCommand;
	HRESULT hr = pCommands->Item(CComVariant(CStringW(L"WWhizNet.Connect.") + CStringW(command)), -1,
		&pCommand);

	if (SUCCEEDED(hr))
	{
		CComVariant variant;
		pCommand->get_Bindings(&variant);

		COleSafeArray sa(variant);

		LONG lLBound, lUBound;
		sa.GetLBound(1, &lLBound);
		sa.GetUBound(1, &lUBound);

		LONG count = lUBound-lLBound+1;

		CComBSTR bstrKeyStr = binding;

		CComBSTR bstrFullStr;
		if (!justText)
			bstrFullStr.Append(L"Global::");
		else
			bstrFullStr.Append(L"Text Editor::");
		bstrFullStr.Append(bstrKeyStr);

		sa.ResizeOneDim(count + 1);
		CComVariant varFullStr = bstrFullStr;
		sa.PutElement(&count, &varFullStr);

		count++;

		hr = pCommand->put_Bindings(sa);
		if (FAILED(hr))
			return false;
	}

	return true;
}


#endif WWHIZ_VSNET
