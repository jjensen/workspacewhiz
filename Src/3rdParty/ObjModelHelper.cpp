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


