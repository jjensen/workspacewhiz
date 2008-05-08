///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizTemplates.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizTemplates/WWhizTemplates.cpp $
// $Date: 2003/01/16 $ $Revision: #10 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizTemplates.h"
#include <direct.h>
#include <io.h>

#ifdef WWHIZ_VC6
#include <initguid.h>
#include "ObjModelGUID.h"
#include <ObjModel\appguid.h>
#include <ObjModel\bldguid.h>
#endif WWHIZ_VC6

WWhizTemplateManager* g_templateManager;
WWhizInterface* g_wwhizInterface;

typedef WWhiz_TemplateException TException;

#ifdef WWHIZ_VC6

/**
**/
static CComPtr<IGenericProject> FindProjectByName(LPCTSTR desiredProjectName)
{
	// Try and retrieve a project by name.
	// First, get a pointer to the dispatch for the Projects collection
	CComPtr<IDispatch> pDispProjects;
	ObjModelHelper::GetInterface()->get_Projects(&pDispProjects);
	CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

	// Get the number of projects in the collection
	long projectCount;
	pProjects->get_Count(&projectCount);

	// Iterate all the projects.
	CComPtr<IGenericProject> pGenProject;
	for (long i = 1; i <= projectCount; i++)
	{
		// Get the next project
		pProjects->Item(CComVariant(i), &pGenProject);
		CComQIPtr<IGenericProject, &IID_IGenericProject> pProject(pGenProject);

		// Get the project name.
		CComBSTR bszStr;
		pProject->get_Name(&bszStr);
		CString projectName = bszStr;

		if (projectName == desiredProjectName)
		{
			return pGenProject;
		}
	}

	pGenProject = NULL;
	return pGenProject;
}


class ProjectHelper
{
public:
	CComQIPtr<IBuildProject, &IID_IBuildProject> m_pBuildProject;
	CComPtr<IConfigurations> m_pConfigurations;
	CComQIPtr<IConfiguration, &IID_IConfiguration> m_pConfig;	// The found configuration

	bool FindProject(const CString& projectName);
	bool FindConfig(const CString& configName);
	int GetNumConfigurations();
	CString GetConfigName(int index);
};


bool ProjectHelper::FindProject(const CString& projectName)
{
	CComPtr<IDispatch> pDispGenericProject;

	// Get the active project if there is no project name as a parameter.
	if (projectName.IsEmpty())
	{
		ObjModelHelper::GetInterface()->get_ActiveProject(&pDispGenericProject);
		if (!pDispGenericProject)
		{
			throw TException(TException::NO_ACTIVE_PROJECT, "There is no active project.");
		}
	}
	else
	{
		// Try and retrieve a project by name.
		pDispGenericProject = FindProjectByName(projectName);
		if (!pDispGenericProject)
		{
			throw TException(TException::INVALID_PROJECT_NAME, "There is no project in the workspace called [%s].", projectName);
		}
	}

	m_pBuildProject = pDispGenericProject;

	m_pBuildProject->get_Configurations(&m_pConfigurations);

	return true;
}


bool ProjectHelper::FindConfig(const CString& configName)
{
	// Get the active config if there is no project name as a parameter.
	if (configName.IsEmpty())
	{
		CComPtr<IDispatch> pDispConfig;
		ObjModelHelper::GetInterface()->get_ActiveConfiguration(&pDispConfig);
		if (!pDispConfig)
		{
			throw TException(TException::NO_ACTIVE_CONFIG, "There is no active configuration.");
		}

		m_pConfig = pDispConfig;

		return true;
	}
	else
	{
		// Try and retrieve a configuration by name.
		long lNumConfigs;
		m_pConfigurations->get_Count(&lNumConfigs);
		for (long j = 1 ; j < lNumConfigs+1 ; j++)
		{
			CComPtr<IDispatch> pDispConfig;
			m_pConfig = pDispConfig;

			// Get each individual configuration
			m_pConfigurations->Item(CComVariant(j), &m_pConfig);

			CComBSTR bszStr;
			m_pConfig->get_Name(&bszStr);
			CString foundName = bszStr;
			if (configName == foundName)
				return true;
		}
	}

	return false;
}


int ProjectHelper::GetNumConfigurations()
{
	long lCount;
	m_pConfigurations->get_Count(&lCount);
	return (int)lCount;
}


CString ProjectHelper::GetConfigName(int index)
{
	// Get the configuration project.
	CComPtr<IConfiguration> pConfig;
	m_pConfigurations->Item(CComVariant(index), &pConfig);
	if (!pConfig)
	{
		return "";
	}

	CComBSTR bszStr;
	pConfig->get_Name(&bszStr);
	CString configName = bszStr;
	return configName;
}

#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET

/**
**/
static CComPtr<EnvDTE::Project> FindProjectByName(LPCTSTR desiredProjectName)
{
	// Get the current project
	CComPtr<EnvDTE::_Solution> pSolution;
	g_pDTE->get_Solution(&pSolution);
	if (!pSolution)
		return NULL;

	CComPtr<EnvDTE::Projects> pProjects;
	pSolution->get_Projects(&pProjects);
	if (!pProjects)
		return NULL;

	CComPtr<EnvDTE::Project> pProject;
	pProjects->Item(CComVariant(desiredProjectName), &pProject);
	return pProject;
}


class ProjectHelper
{
public:
	CComPtr<EnvDTE::_Solution> m_pSolution;
	CComPtr<EnvDTE::SolutionBuild> m_pSolutionBuild;
	CComPtr<EnvDTE::SolutionConfiguration> m_pSolutionConfiguration;
	CComPtr<EnvDTE::Project> m_pProject;
	CComPtr<EnvDTE::ConfigurationManager> m_pConfigurationManager;
	CComPtr<EnvDTE::Configuration> m_pConfiguration;
//	CComQIPtr<IBuildProject, &IID_IBuildProject> m_pBuildProject;
//	CComPtr<IConfigurations> m_pConfigurations;
//	CComQIPtr<IConfiguration, &IID_IConfiguration> m_pConfig;	// The found configuration

	bool FindProject(const CString& projectName);
	bool FindConfig(const CString& configName);
	int GetNumConfigurations();
	CString GetConfigName(int index);
};


CComPtr<EnvDTE::Project> GetActiveProject()
{
	if (!g_pDTE)
		return NULL;

	// Get the current project
	CComPtr<EnvDTE::_Solution> pSolution;
	g_pDTE->get_Solution(&pSolution);
	if (!pSolution)
		return NULL;

	CComBSTR bstrConfig;

	CComPtr<IDispatch> pActiveProjectDisp;
	CComPtr<EnvDTE::Project> pActiveProject;
	CComVariant varActiveSolutionProjects;
	g_pDTE->get_ActiveSolutionProjects(&varActiveSolutionProjects);
	if(varActiveSolutionProjects.vt == (VT_ARRAY|VT_VARIANT))
	{
		CComVariant varProj;

		LONG lLBound, lUBound;
		SafeArrayGetLBound(varActiveSolutionProjects.parray, 1, &lLBound);
		SafeArrayGetUBound(varActiveSolutionProjects.parray, 1, &lUBound);

		if((lUBound - lLBound) != 0)
		{
			return NULL;
		}

		SafeArrayGetElement(varActiveSolutionProjects.parray, &lLBound, &varProj);
		pActiveProjectDisp = varProj.pdispVal;
		pActiveProject = pActiveProjectDisp;
	}

	return pActiveProject;
}


bool ProjectHelper::FindProject(const CString& projectName)
{
//	CComPtr<IDispatch> pDispGenericProject;
	CComPtr<EnvDTE::Project> pProject;

	// Get the active project if there is no project name as a parameter.
	if (projectName.IsEmpty())
	{
		pProject = GetActiveProject();
		if (!pProject)
		{
			throw TException(TException::NO_ACTIVE_PROJECT, "There is no active project.");
		}
	}
	else
	{
		// Try and retrieve a project by name.
		pProject = FindProjectByName(projectName);
		if (!pProject)
		{
			throw TException(TException::INVALID_PROJECT_NAME, "There is no project in the workspace called [%s].", projectName);
		}
	}

	m_pProject = pProject;
	m_pProject->get_ConfigurationManager(&m_pConfigurationManager);

	return true;
}


bool ProjectHelper::FindConfig(const CString& configName)
{
	CComPtr<EnvDTE::Configuration> pConfiguration;

	// Get the active config if there is no project name as a parameter.
	if (configName.IsEmpty())
	{
		m_pConfigurationManager->get_ActiveConfiguration(&pConfiguration);
		if (!pConfiguration)
		{
			throw TException(TException::NO_ACTIVE_CONFIG, "There is no active configuration.");
		}

		m_pConfiguration = pConfiguration;

		return true;
	}
	else
	{
		m_pConfigurationManager->Item(CComVariant(configName), CComBSTR(), &pConfiguration);
		m_pConfiguration = pConfiguration;
	}

	return false;
}


int ProjectHelper::GetNumConfigurations()
{
	long lCount;
	m_pConfigurationManager->get_Count(&lCount);
	return (int)lCount;
}


CString ProjectHelper::GetConfigName(int index)
{
	// Get the configuration project.
	CComPtr<EnvDTE::Configuration> pConfig;
	m_pConfigurationManager->Item(CComVariant(index), CComBSTR(), &pConfig);
	if (!pConfig)
	{
		return "";
	}

	CComBSTR bszStr;
	pConfig->get_ConfigurationName(&bszStr);
	CString configName = bszStr;
	return configName;
}


#endif WWHIZ_VSNET
			
/**
**/
static bool GetCurrentFilename(CString &filename)
{
	ObjModelHelper objModel;
	if (!objModel.GetActiveDocument())
	{
		filename.Empty();
		return false;
	}

	filename = objModel.GetFilename();
	return true;
}


/**
**/
static void MakeDir(LPCTSTR newPath)
{
	CString path;

	// Get the full name.
	char fileBuffer[_MAX_PATH];
	char* ret = _fullpath(fileBuffer, newPath, _MAX_PATH);

	path = fileBuffer;

	// Replace all \ with /.
	path.Replace('\\', '/');

	// Remove the filename.
	int lastSlashPos = path.ReverseFind('/');
	path = path.Left(lastSlashPos + 1);

	// Skip the c:/ part.
	int slashPos = path.Find('/');
	slashPos = path.Find('/', slashPos + 1);

	while (slashPos != -1)
	{
		_mkdir(path.Left(slashPos));
		slashPos = path.Find('/', slashPos + 1);
	}
}


/**
**/
static void SplitPath(const CString& fullPath, CString& path, CString& title, CString& ext)
{
	// Split the path.
	_splitpath(fullPath, NULL, path.GetBuffer(MAX_PATH),
		title.GetBuffer(MAX_PATH), ext.GetBuffer(MAX_PATH));
	path.ReleaseBuffer();
	title.ReleaseBuffer();
	ext.ReleaseBuffer();

	// Remove the dot from the ext.
	if (!ext.IsEmpty())
		ext = ext.Mid(1);
}


static bool AsciiToInteger(LPCTSTR nptr, int& total)
{
	int c;              // current char
	int sign;           // if '-', then negative, otherwise positive
	
	c = (int)(BYTE)*nptr++;
	if (c == 0)
		return false;
	sign = c;           /* save sign indication */
	if (c == '-' || c == '+')
		c = (int)(BYTE)*nptr++;    /* skip sign */
	
	total = 0;
	
	while (isdigit(c))
	{
		total = 10 * total + (c - '0');     /* accumulate digit */
		c = (int)(BYTE)*nptr++;    /* get next char */
	}

	if (c == 0)
	{
		if (sign == '-')
			total = -total;
		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Add
///////////////////////////////////////////////////////////////////////////////
class AddCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString varName = args.Get(0);
		CString value = curTemplate->GetEntry(varName);
		CString addAmount = args.Get(1);
		int valueInt;
		int addAmountInt;

		if (AsciiToInteger(value, valueInt)  &&  AsciiToInteger(addAmount, addAmountInt))
		{
			valueInt += addAmountInt;
			value.Format("%d", valueInt);
			curTemplate->SetEntry(varName, value);
		}
	}
};


///////////////////////////////////////////////////////////////////////////////
// Call
///////////////////////////////////////////////////////////////////////////////
class CallCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		///////////////////////////////////////////////////////////////////
		// Call another template
		///////////////////////////////////////////////////////////////////
		CString templateName = args.Get(0);

		WWhizTemplateGroup& parent = curTemplate->GetParent();
		int templateIndex = parent.Find(templateName);
		if (templateIndex == -1)
		{
			throw TException(TException::INVALID_TEMPLATE_NAME, "The template [%s] to be called does not exist.", templateName);
		}

		// Flush...
		curTemplate->FlushText();

		WWhizTemplate* callTemplate = parent.Get(templateIndex);
		callTemplate->ResetDictionaryDefaults();

		// Set all the parameters
		int index = 1;
		while (true)
		{
			// Read the parameter name.
			CString paramName = args.Get(index++);
			if (paramName.IsEmpty())
				break;

			// Read the value.
			CString value = args.Get(index++);

			// Set the dictionary entry.
			callTemplate->SetEntry(paramName, value);
		}

		callTemplate->SetEntry("__Parent", curTemplate->GetName());

		callTemplate->Parse();

		CString returnValue = callTemplate->GetEntry("__Return");

		args.Return(returnValue);
	}
};


///////////////////////////////////////////////////////////////////////////////
// Caret
///////////////////////////////////////////////////////////////////////////////
class CaretCommand : public WWhizTemplateCommand
{
public:
	virtual void Create(WWhizTemplate* curTemplate)
	{
		m_fullName.Empty();
		m_curX = m_curY = -1;
	}

	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Flush...
		curTemplate->FlushText();

		// Pick out the current caret position.
		ObjModelHelper helper;
		helper.GetActiveDocument();
		if (helper.IsTextDocument())
		{
			// Save the document's name.
			m_fullName = helper.GetFilename();

			// Save the last coordinate.
			helper.GetPosition(m_curX, m_curY);

			// Return a character to find.
			args.Return("~~~CARET~~~");
		}
	}

	virtual void Destroy(WWhizTemplate* curTemplate)
	{
		if (m_curX != -1  &&  m_curY != -1)
		{
			ObjModelHelper helper;
			helper.GetActiveDocument();
			bool isDifferent = true;
			if (helper.IsTextDocument())
			{
				// Save the document's name.
				CString fullName = helper.GetFilename();
				if (m_fullName == fullName)
					isDifferent = false;
			}

			// Add the new document to the documents list.
			if (isDifferent)
			{
				if (!helper.OpenDocument(m_fullName))
				{
					return;
				}
			}

			helper.GetActiveDocument();
			helper.MoveTo(m_curY, 1, false);
			if (!helper.FindText("~~~CARET~~~", "MatchForward"))
			{
				AfxMessageBox("Error returning to the requested caret position.");
			}
			else
			{
				helper.Delete();
			}
		}
	}

protected:
	CString m_fullName;					// The document's path name.
	long m_curX;						// The new X
	long m_curY;						// The new Y
};


///////////////////////////////////////////////////////////////////////////////
// ConfigAddFileSettings
///////////////////////////////////////////////////////////////////////////////
class ConfigAddFileSettingsCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the project name.
		CString projectName = args.Get(0);
		if (projectName == '~')
			projectName.Empty();
		ProjectHelper helper;
		helper.FindProject(projectName);

		CString configName = args.Get(1);
		if (configName == '~')
			configName.Empty();
		helper.FindConfig(configName);

		CString filename = args.Get(2);
		if (filename.IsEmpty())
		{
			throw TException(TException::FILENAME_REQUIRED, "Filename is required as parameter 3.");
		}

		CString settings = args.Get(3);
		if (settings.IsEmpty())
		{
			throw TException(TException::FILENAME_REQUIRED, "File settings are required as parameter 4.");
		}

#ifdef WWHIZ_VSNET
		CString err;
		err.Format("Visual Studio .NET support for the ConfigAddFileSettings command "
				"has not been implemented yet.");
		AfxMessageBox(err);
#endif WWHIZ_VSNET
#ifdef WWHIZ_VC6
		// Get DevStudio version.  Only DevStudio 6.0 can use this command.
		CString version = ObjModelHelper::GetVersion();
		if (version != "6")
		{
			helper.m_pConfig->AddFileSettings(CComBSTR(filename), CComBSTR(settings), CComVariant());
		}
		else
		{
			CString err;
			err.Format("Developer Studio 5.0 has no support for adding individual file settings.  "
					"Please verify the following settings are added to project [%s] "
					"(~ is the active project), configuration [%s], file [%s].  "
					"Settings: [%s]", projectName, configName, filename, settings);
			AfxMessageBox(err);
		}
#endif WWHIZ_VC6
	}
};


///////////////////////////////////////////////////////////////////////////////
// ConfigAddToolSettings
///////////////////////////////////////////////////////////////////////////////
class ConfigAddToolSettingsCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the project name.
		CString projectName = args.Get(0);
		if (projectName == '~')
			projectName.Empty();
		ProjectHelper helper;
		helper.FindProject(projectName);

		CString configName = args.Get(1);
		if (configName == '~')
			configName.Empty();
		helper.FindConfig(configName);

		CString filename = args.Get(2);
		if (filename.IsEmpty())
		{
			throw TException(TException::FILENAME_REQUIRED, "Filename is required as parameter 3.");
		}

		CString settings = args.Get(3);
		if (settings.IsEmpty())
		{
			throw TException(TException::FILENAME_REQUIRED, "File settings are required as parameter 4.");
		}

#ifdef WWHIZ_VC6
		helper.m_pConfig->AddToolSettings(CComBSTR(filename), CComBSTR(settings), CComVariant());
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
		CString err;
		err.Format("Visual Studio .NET support for the ConfigAddToolsSettings command "
				"has not been implemented yet.");
		AfxMessageBox(err);
#endif WWHIZ_VSNET
	}
};


///////////////////////////////////////////////////////////////////////////////
// ConfigCount
///////////////////////////////////////////////////////////////////////////////
class ConfigCountCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the project name.
		CString projectName = args.Get(0);
		if (projectName == '~')
			projectName.Empty();
		ProjectHelper helper;
		helper.FindProject(projectName);

		// Return the count.
		args.Return(helper.GetNumConfigurations());
	}
};


///////////////////////////////////////////////////////////////////////////////
// ConfigGet
///////////////////////////////////////////////////////////////////////////////
class ConfigGetCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the project name.
		CString projectName = args.Get(0);
		if (projectName == '~')
			projectName.Empty();
		ProjectHelper helper;
		helper.FindProject(projectName);

		// Get the project index.
		int index;
		if (!AsciiToInteger(args.Get(1), index))
		{
			throw TException(TException::MISSING_VARIABLE, "A numeric index is required as parameter 2.");
		}

		// Get the configuration project.
		args.Return(helper.GetConfigName(index));
	}
};


///////////////////////////////////////////////////////////////////////////////
// ConfigRemoveFileSettings
///////////////////////////////////////////////////////////////////////////////
class ConfigRemoveFileSettingsCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the project name.
		CString projectName = args.Get(0);
		if (projectName == '~')
			projectName.Empty();
		ProjectHelper helper;
		helper.FindProject(projectName);

		CString configName = args.Get(1);
		if (configName == '~')
			configName.Empty();
		helper.FindConfig(configName);

		CString filename = args.Get(2);
		if (filename.IsEmpty())
		{
			throw TException(TException::FILENAME_REQUIRED, "Filename is required as parameter 3.");
		}

		CString settings = args.Get(3);
		if (settings.IsEmpty())
		{
			throw TException(TException::FILENAME_REQUIRED, "File settings are required as parameter 4.");
		}

#ifdef WWHIZ_VSNET
		CString err;
		err.Format("Visual Studio .NET support for the ConfigRemoveFileSettings command "
				"has not been implemented yet.");
		AfxMessageBox(err);
#endif WWHIZ_VSNET
#ifdef WWHIZ_VC6
		CString version = ObjModelHelper::GetVersion();
		if (version != "6")
		{
			helper.m_pConfig->RemoveFileSettings(CComBSTR(filename), CComBSTR(settings), CComVariant());
		}
		else
		{
			CString err;
			err.Format("Developer Studio 5.0 has no support for removing individual file settings.  "
					"Please verify the following settings are removed from project [%s] "
					"(~ is the active project), configuration [%s], file [%s].  "
					"Settings: [%s]", projectName, configName, filename, settings);
			AfxMessageBox(err);
		}
#endif WWHIZ_VC6
	}
};


///////////////////////////////////////////////////////////////////////////////
// ConfigRemoveToolSettings
///////////////////////////////////////////////////////////////////////////////
class ConfigRemoveToolSettingsCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the project name.
		CString projectName = args.Get(0);
		if (projectName == '~')
			projectName.Empty();
		ProjectHelper helper;
		helper.FindProject(projectName);

		CString configName = args.Get(1);
		if (configName == '~')
			configName.Empty();
		helper.FindConfig(configName);

		CString filename = args.Get(2);
		if (filename.IsEmpty())
		{
			throw TException(TException::FILENAME_REQUIRED, "Filename is required as parameter 3.");
		}

		CString settings = args.Get(3);
		if (settings.IsEmpty())
		{
			throw TException(TException::FILENAME_REQUIRED, "File settings are required as parameter 4.");
		}

#ifdef WWHIZ_VSNET
		CString err;
		err.Format("Visual Studio .NET support for the ConfigRemoveToolSettings command "
				"has not been implemented yet.");
		AfxMessageBox(err);
#endif WWHIZ_VSNET
#ifdef WWHIZ_VC6
		helper.m_pConfig->RemoveToolSettings(CComBSTR(filename), CComBSTR(settings), CComVariant());
#endif WWHIZ_VC6
	}
};


///////////////////////////////////////////////////////////////////////////////
// Date
///////////////////////////////////////////////////////////////////////////////
class DateCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString format = args.Get(0);
		LPCTSTR formatPtr = format.IsEmpty() ? NULL : (LPCTSTR)format;

		int dateLen =
			::GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, formatPtr, NULL, 0);
		CString out;
		LPTSTR outBuf = out.GetBuffer(dateLen);
		::GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, formatPtr, outBuf, dateLen);
		out.ReleaseBuffer();
		args.Return(out);
	}
};


///////////////////////////////////////////////////////////////////////////////
// Dialog
///////////////////////////////////////////////////////////////////////////////
class DialogCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		if (args.Get(0).GetLength() > 0)
		{
			WWhizTemplateDialog* dialog = curTemplate->GetTemplateDialog();
			if (dialog)
			{
				bool nextOn = false;
				bool previousOn = false;
				bool finishOn = false;
				if (args.Get(1) == "1")
					nextOn = true;
				if (args.Get(2) == "1")
					previousOn = true;
				if (args.Get(3) == "1")
					finishOn = true;
				CString returnValue =
						dialog->Run(*curTemplate, args.Get(0), nextOn, previousOn, finishOn);
				args.Return(returnValue);
			}
		}
	}
};


///////////////////////////////////////////////////////////////////////////////
// Env
///////////////////////////////////////////////////////////////////////////////
class EnvCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Pick out the environment string.
		CString env = args.Get(0);

		// See if we can resolve it.
		DWORD size = ::GetEnvironmentVariable(env, NULL, 0);
		if (size == 0)
			return;

		CString buffer;
		LPTSTR buf = buffer.GetBufferSetLength(size - 1);
		DWORD ret = ::GetEnvironmentVariable(env, buf, size);
		buffer.ReleaseBuffer(size - 1);
		args.Return(buffer);
	}
};


///////////////////////////////////////////////////////////////////////////////
// ExecuteCommand
///////////////////////////////////////////////////////////////////////////////
class ExecuteCommandCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString command = args.Get(0);
		if (command.IsEmpty())
		{
			throw TException(TException::MISSING_COMMAND, "Parameter 1 should be a DevStudio-known command.");
		}

		CString arguments = args.Get(1);

		curTemplate->FlushText();
		if (!ObjModelHelper::ExecuteCommand(command, arguments))
		{
			throw TException(TException::UNABLE_TO_EXECUTE_COMMAND, "Command [%s] is not a known Visual Studio command.", command);
		}
	}
};


///////////////////////////////////////////////////////////////////////////////
// FileExt
///////////////////////////////////////////////////////////////////////////////
class FileExtCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the current file path.
		CString filename;
		GetCurrentFilename(filename);
		CString path;
		CString title;
		CString ext;
		SplitPath(filename, path, title, ext);
		args.Return(ext);
	}
};


///////////////////////////////////////////////////////////////////////////////
// FileNew
///////////////////////////////////////////////////////////////////////////////
class FileNewCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString filename = args.Get(0);
		if (filename.IsEmpty())
		{
			throw TException(TException::FILENAME_REQUIRED, "A filename of the new file is required as parameter 1.");
		}

		CString fullName;

		CString status = args.Get(1);
		status.MakeLower();

		// Is the filename absolute?
		if (status.IsEmpty()  ||  status == "abs")
		{
			fullName = filename;
		}

		// Is the filename relative to the current project's dsp file?
		else if (status == "dsp")
		{
#ifdef WWHIZ_VC6
			CComPtr<IDispatch> pDispProject;
			CComQIPtr<IGenericProject, &IID_IGenericProject> pProject;
			ObjModelHelper::GetInterface()->get_ActiveProject(&pDispProject);

			if (!pDispProject)
			{
				throw TException(TException::NO_ACTIVE_PROJECT, "There is no active project to create the file [%s] relative to.", filename);
			}

			pProject = pDispProject;
			if (!pProject)
			{
				throw TException(TException::NO_ACTIVE_PROJECT, "There is no active project to create the file [%s] relative to.", filename);
			}
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
			CComPtr<EnvDTE::Project> pProject = GetActiveProject();
			if (!pProject)
			{
				throw TException(TException::NO_ACTIVE_PROJECT, "There is no active project to create the file [%s] relative to.", filename);
			}
#endif WWHIZ_VSNET

			// Get the project name.
			CComBSTR bszStr;
			pProject->get_FullName(&bszStr);
			CString projectName = bszStr;

			// Get the project path.
			int pos = projectName.ReverseFind('\\');
			projectName = projectName.Left(pos + 1);

			fullName = projectName + filename;
		}

		// Is the filename relative to the current working directory?
		else if (status == "rel")
		{
			fullName = ".\\" + filename;
		}
		else
		{
			throw TException(TException::PATH_STATUS_INVALID, "The specified path status [%s] is invalid.", status);
		}

		// Make the directory.
		MakeDir(fullName);
/* WON'T WORK
		WWhizTemplateCommandArgs* args = curTemplate->CreateArgs();
		WWhizTemplateCommand* command = g_templateManager->GetCommand("MakeDir");
		if (command)
			command->Run(curTemplate, *args);
		curTemplate->DestroyArgs(args);
*/

		// See if it already exists.
		bool overwrite = true;
		if (_access(fullName, 00) != -1)
		{
			// The file exists.  Prompt the user!
			CString msg;
			msg.Format("The file [%s] about to be saved by the template already exists.  Overwrite?", fullName);
			if (AfxMessageBox(msg, MB_YESNO) == IDNO)
				overwrite = false;
		}
		
#ifdef WWHIZ_VC6
		CComPtr<IDispatch> pDispDocument;
		CComPtr<IDispatch> pDispDocuments;
		CComQIPtr<IDocumentsVC, &IID_IDocumentsVC> pDocumentsVC;
		CComQIPtr<IDocumentsEVC, &IID_IDocumentsEVC> pDocumentsEVC;
		ObjModelHelper::GetInterface()->get_Documents(&pDispDocuments);
		pDocumentsVC = pDispDocuments;
		pDocumentsEVC = pDispDocuments;
#endif WWHIZ_VC6

		if (overwrite)
		{
#ifdef WWHIZ_VC6
			// Add the new document to the documents list.
			if (pDocumentsVC)
			{
				if (FAILED(pDocumentsVC->Add(CComBSTR("Text"), CComVariant(), &pDispDocument)))
				{
					throw TException(TException::UNABLE_TO_CREATE_NEW_FILE, "Unable to add the new file to the active document list.");
				}
			}
			else if (pDocumentsEVC)
			{
				if (FAILED(pDocumentsEVC->Add(CComBSTR("Text"), CComVariant(), &pDispDocument)))
				{
					throw TException(TException::UNABLE_TO_CREATE_NEW_FILE, "Unable to add the new file to the active document list.");
				}
			}
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
			CComPtr<EnvDTE::ItemOperations> pItemOperations;
			if (FAILED(g_pDTE->get_ItemOperations(&pItemOperations)))
				return;

			EnvDTE::WindowPtr window;
			pItemOperations->NewFile(CComBSTR("General\\Text File"), CComBSTR(fullName),
				CComBSTR(EnvDTE::vsViewKindTextView), &window);
#endif WWHIZ_VSNET

			// Save the document.
			ObjModelHelper objModel;
			objModel.GetActiveDocument();
			if (!objModel.SaveDocument(fullName))
			{
				// Unable to save the document.
				throw TException(TException::UNABLE_TO_CREATE_NEW_FILE, "Unable to create the file [%s].", fullName);
			}
		}
		else
		{
			ObjModelHelper objModel;
			if (!objModel.OpenDocument(fullName))
			{
				throw TException(TException::UNABLE_TO_CREATE_NEW_FILE, "Unable to add the new file to the active document list.");
			}
		}

//		curTemplate->RefreshFileInfo();
	}
};


///////////////////////////////////////////////////////////////////////////////
// FileNewText
///////////////////////////////////////////////////////////////////////////////
class FileNewTextCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		ObjModelHelper objModel;
#ifdef WWHIZ_VSNET
		objModel.ExecuteCommand("File.NewFile", "/t:\"General\\Text File\"");
#endif WWHIZ_VSNET

#ifdef WWHIZ_VC6
		objModel.ExecuteCommand("NewText", "");
#endif WWHIZ_VC6
	}
};


///////////////////////////////////////////////////////////////////////////////
// FilePath
///////////////////////////////////////////////////////////////////////////////
class FilePathCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the current file path.
		CString filename;
		GetCurrentFilename(filename);
		args.Return(filename);
	}
};


///////////////////////////////////////////////////////////////////////////////
// FileSave
///////////////////////////////////////////////////////////////////////////////
class FileSaveCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Flush...
		curTemplate->FlushText();

		// Save the document.
		CString filename;
		GetCurrentFilename(filename);
		if (!filename.IsEmpty())
		{
			ObjModelHelper objModel;
			objModel.GetActiveDocument();
			if (!objModel.SaveDocument(filename))
			{
				// Failed.
				throw TException(TException::UNABLE_TO_SAVE_FILE, "Unable to save the file [%s].", filename);
			}
		}
		else
		{
			throw TException(TException::NO_ACTIVE_FILE, "There is no active file open to save.");
		}
	}
};


///////////////////////////////////////////////////////////////////////////////
// FileTitle
///////////////////////////////////////////////////////////////////////////////
class FileTitleCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the current file path.
		CString filename;
		GetCurrentFilename(filename);
		CString path;
		CString title;
		CString ext;
		SplitPath(filename, path, title, ext);
		args.Return(title);
	}
};


///////////////////////////////////////////////////////////////////////////////
// Find
///////////////////////////////////////////////////////////////////////////////
class FindCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString var = args.Get(0);
		if (var.IsEmpty())
		{
			throw TException(TException::MISSING_VARIABLE, "A variable name is required as parameter 1.");
		}
		CString string = args.Get(1);
		if (string.IsEmpty())
		{
			throw TException(TException::MISSING_FIND_STRING, "A string to find is required as parameter 2.");
		}

		DWORD flags = 0;
		
		int index = 2;
		while (true)
		{
			CString flagStr = args.Get(index++);
			if (flagStr.IsEmpty())
				break;

			flagStr.MakeLower();
#ifdef WWHIZ_VC6
			if (flagStr == "matchforward")
				flags |= dsMatchForward;
			else if (flagStr == "matchbackward")
				flags |= dsMatchBackward;
			else if (flagStr == "matchfromstart")
				flags |= dsMatchFromStart;
			else if (flagStr == "matchword")
				flags |= dsMatchWord;
			else if (flagStr == "matchcase")
				flags |= dsMatchCase;
			else if (flagStr == "matchregexp")
				flags |= dsMatchRegExp;
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
			if (flagStr == "matchforward")
				flags |= 0;
			else if (flagStr == "matchbackward")
				flags |= EnvDTE::vsFindOptionsBackwards;
			else if (flagStr == "matchfromstart")
				flags |= EnvDTE::vsFindOptionsFromStart;
			else if (flagStr == "matchword")
				flags |= EnvDTE::vsFindOptionsMatchWholeWord;
			else if (flagStr == "matchcase")
				flags |= EnvDTE::vsFindOptionsMatchCase;
			else if (flagStr == "matchregexp")
				flags |= EnvDTE::vsFindOptionsRegularExpression;
#endif WWHIZ_VSNET
			else
			{
				throw TException(TException::INVALID_FIND_FLAG, "The specified flag [%s] is an invalid find flag.", flagStr);
			}
		}

		// Flush...
		curTemplate->FlushText();

		// Get the active text selection.
		ObjModelHelper helper;
		helper.GetActiveDocument();
		if (!helper.IsTextDocument())
		{
			throw TException(TException::NO_ACTIVE_FILE, "There is no active file to perform the search within.");
		}
		
		bool found = helper.FindText(string, flags);
		curTemplate->SetEntry(var, found ? "1" : "0");
	}
};


///////////////////////////////////////////////////////////////////////////////
// GetPos
///////////////////////////////////////////////////////////////////////////////
class GetPosCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the variable names.
		CString xVar = args.Get(0);
		if (xVar.IsEmpty())
		{
			throw TException(TException::MISSING_VARIABLE, "A variable name is required as parameter 1.");
		}

		CString yVar = args.Get(1);
		if (yVar.IsEmpty())
		{
			throw TException(TException::MISSING_FIND_STRING, "A string to find is required as parameter 2.");
		}

		// Flush...
		curTemplate->FlushText();

		// Pick out the current caret position.
		ObjModelHelper helper;
		helper.GetActiveDocument();
		if (helper.IsTextDocument())
		{
			// Save the last coordinate.
			long curX;
			long curY;
			helper.GetPosition(curX, curY);

			CString curXStr;
			curXStr.Format("%d", curX);
			curTemplate->SetEntry(xVar, curXStr);

			CString curYStr;
			curYStr.Format("%d", curY);
			curTemplate->SetEntry(yVar, curYStr);
		}
	}
};


///////////////////////////////////////////////////////////////////////////////
// Goto
///////////////////////////////////////////////////////////////////////////////
/**
**/
static CString ParseToken(LPCTSTR& codePtr)
{
	// Remove leading whitespace.
	while (*codePtr != 0  &&  *codePtr != '\n'  &&  *codePtr == ' ')
	{
		codePtr++;
	}
		
	// Get the parameter.
	bool inQuote = false;
	if (*codePtr == '"')
	{
		inQuote = true;
		codePtr++;
	}

	// Remember where the start is.
	LPCTSTR startPtr = codePtr;

	while (*codePtr != 0)
	{
		if (*codePtr == '\n')
		{
			if (inQuote)
			{
				throw TException(TException::UNEXPECTED_EOL, "Unexpected end of line while parsing a quoted string.");
			}
			break;
		}
		else if (*codePtr == '"')
		{
			codePtr++;
			if (inQuote)
				break;
		}
		else if (*codePtr == ' ')
		{
			if (!inQuote)
				break;
			codePtr++;
		}
		else
		{
			codePtr++;
		}
	}

	CString out;
	int len = codePtr - startPtr;
	LPTSTR buf = out.GetBufferSetLength(len);
	_tcsncpy(buf, startPtr, len);
	out.ReleaseBuffer(len);

	return out;
}


/**
**/
static void SkipToEol(LPCTSTR& codePtr)
{
	while (*codePtr)
	{
		if (*codePtr == '\n')
			break;
		codePtr++;
	}
}


class GotoCommand : public WWhizTemplateCommand
{
public:
	virtual void Create(WWhizTemplate* curTemplate)
	{
		const CString& code = curTemplate->GetCode();
		LPCTSTR codePtr = code;
		LPCTSTR bolPtr = codePtr;
		while (*codePtr)
		{
			if (codePtr == bolPtr  &&  codePtr[0] == '!'  &&  codePtr[1] == '!')
			{
				codePtr += 2;
				CString tokenName = ParseToken(codePtr);
				tokenName.MakeLower();
				if (tokenName == "label")
				{
					CString labelName = ParseToken(codePtr);
					labelName.MakeLower();
					SkipToEol(codePtr);		// Leaves us on the \n

					Info info;
					info.m_label = labelName;
					info.m_codePtr = codePtr + 1;
					m_labels.AddTail(info);
				}
				else
				{
					SkipToEol(codePtr);		// Leaves us on the \n
				}
			}
			else if (*codePtr == '\n')
			{
				codePtr++;
				bolPtr = codePtr;
			}
			else
			{
				codePtr++;
			}

		}
	}

	virtual void Destroy(WWhizTemplate* curTemplate)
	{
		m_labels.RemoveAll();
	}

	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString labelName = args.Get(0);
		labelName.MakeLower();
		POSITION pos = m_labels.GetHeadPosition();
		while (pos)
		{
			Info& info = m_labels.GetNext(pos);
			if (info.m_label == labelName)
			{
				curTemplate->SetCodePtr(info.m_codePtr);
				return;
			}
		}

		throw TException(TException::INVALID_PARAMETER, "Unknown label in !!Goto %s.", labelName);
	}

protected:
	struct Info
	{
		CString m_label;
		LPCTSTR m_codePtr;
	};
	WList<Info> m_labels;
};

		
///////////////////////////////////////////////////////////////////////////////
// GUID
///////////////////////////////////////////////////////////////////////////////
class GUIDCommand : public WWhizTemplateCommand
{
public:
	void GetFormattedGuid(CString& rString, int whichFormat)
	{
	}

	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// create random GUID
		GUID guid;    // generated GUID
		guid = GUID_NULL;
		::CoCreateGuid(&guid);
		if (guid == GUID_NULL)
		{
			throw TException(TException::GENERAL_FAILURE, "Unable to create GUID.");
		}

		int type;
		if (args.Get(0).IsEmpty())
			type = 0;
		else
		{
			if (!AsciiToInteger(args.Get(0), type))
			{
				throw TException(TException::MISSING_VARIABLE, "A numeric type is required as parameter 1.");
			}

			if (type > 4)
			{
				throw TException(TException::INVALID_PARAMETER, "A numeric type between 0 and 4 is required as parameter 1.");
			}
		}

		static LPCTSTR formats[] =
		{
			"%08lX_%04X_%04x_%02X%02X_%02X%02X%02X%02X%02X%02X",
			"{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}\r\n",
			"// {%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}\r\nIMPLEMENT_OLECREATE(<<class>>, <<external_name>>, \r\n0x%lx, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x);\r\n",
			"// {%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}\r\nDEFINE_GUID(<<name>>, \r\n0x%lx, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x);\r\n",
			"// {%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}\r\nstatic const GUID <<name>> = \r\n{ 0x%lx, 0x%x, 0x%x, { 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x } };\r\n",
		};

		// then format into destination
		CString str;
		str.Format(formats[type],
			// first copy...
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7],
			// second copy...
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

		args.Return(str);
	}
};

		
///////////////////////////////////////////////////////////////////////////////
// Label
///////////////////////////////////////////////////////////////////////////////
class LabelCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Ignore.  Prescanned by Goto
	}
};

		
///////////////////////////////////////////////////////////////////////////////
// MakeDir
///////////////////////////////////////////////////////////////////////////////
class MakeDirCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString dir = args.Get(0);
		TCHAR ch = dir[dir.GetLength() - 1];
		if (ch != '\\'  &&  ch != '/')
			dir += "\\";
		MakeDir(dir);
	}
};

		
///////////////////////////////////////////////////////////////////////////////
// MessageBox
///////////////////////////////////////////////////////////////////////////////
class MessageBoxCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString message = args.Get(0);

		CString type = args.Get(1);
		type.MakeLower();

		DWORD flags = 0;
		if (type.IsEmpty())
			flags = MB_OK;
		else if (type == "abortretryignore")
			flags = MB_ABORTRETRYIGNORE;
		else if (type == "ok")
			flags = MB_OK;
		else if (type == "okcancel")
			flags = MB_OKCANCEL;
		else if (type == "retrycancel")
			flags = MB_RETRYCANCEL;
		else if (type == "yesno")
			flags = MB_YESNO;
		else if (type == "yesnocancel")
			flags = MB_YESNOCANCEL;
		else
		{
			throw TException(TException::INVALID_PARAMETER, "Invalidate type [%s] for message box.", type);
		}

		int retCode = AfxMessageBox(message, flags);
		CString retStr;
		switch (retCode)
		{
			case IDABORT:	retStr = "abort";	break;
			case IDCANCEL:	retStr = "cancel";	break;
			case IDIGNORE:	retStr = "ignore";	break;
			case IDNO:		retStr = "no";		break;
			case IDOK:		retStr = "ok";		break;
			case IDRETRY:	retStr = "retry";	break;
			case IDYES:		retStr = "yes";		break;
		}

		CString var = args.Get(2);
		if (!var.IsEmpty())
		{
			curTemplate->SetEntry(var, retStr);
		}

		args.Return(retStr);
	}
};


///////////////////////////////////////////////////////////////////////////////
// MoveTo
///////////////////////////////////////////////////////////////////////////////
class MoveToCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the variables.
		CString xStr = args.Get(0);
		int x;
		if (!AsciiToInteger(xStr, x))
		{
			throw TException(TException::MISSING_VARIABLE, "A numeric X coordinate is required as parameter 1.");
		}

		CString yStr = args.Get(1);
		int y;
		if (!AsciiToInteger(yStr, y))
		{
			throw TException(TException::MISSING_VARIABLE, "A numeric Y coordinate is required as parameter 2.");
		}

		bool flags = false;
		CString flagStr = args.Get(2);
		flagStr.MakeLower();
		if (flagStr == "extend")
		{
			flags = true;
		}
		else if (!flagStr.IsEmpty())
		{
			throw TException(TException::MISSING_VARIABLE, "Parameter 3 is incorrect.");
		}

		// Flush...
		curTemplate->FlushText();

		ObjModelHelper helper;
		helper.GetActiveDocument();
		if (helper.IsTextDocument())
		{
			helper.MoveTo(y, x, flags);
		}
	}
};


///////////////////////////////////////////////////////////////////////////////
// ProjectCount
///////////////////////////////////////////////////////////////////////////////
class ProjectCountCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
#ifdef WWHIZ_VC6
		// First, get a pointer to the dispatch for the Projects collection
		CComPtr<IDispatch> pDispProjects;
		ObjModelHelper::GetInterface()->get_Projects(&pDispProjects);
		CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

		// Get the number of projects in the collection
		long projectCount;
		pProjects->get_Count(&projectCount);

		// Return the count.
		args.Return(projectCount);
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
		// Get the current project
		CComPtr<EnvDTE::_Solution> pSolution;
		g_pDTE->get_Solution(&pSolution);
		if (!pSolution)
			return;

		CComPtr<EnvDTE::Projects> pProjects;
		pSolution->get_Projects(&pProjects);
		if (!pProjects)
			return;

		long count;
		pProjects->get_Count(&count);

		args.Return(count);
#endif WWHIZ_VSNET
	}
};


///////////////////////////////////////////////////////////////////////////////
// ProjectGet
///////////////////////////////////////////////////////////////////////////////
class ProjectGetCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the project index.
		int index;
		if (!AsciiToInteger(args.Get(0), index))
		{
			throw TException(TException::MISSING_VARIABLE, "A numeric index is required as parameter 1.");
		}

#ifdef WWHIZ_VC6
		// Try and retrieve a project by name.
		// First, get a pointer to the dispatch for the Projects collection
		CComPtr<IDispatch> pDispProjects;
		ObjModelHelper::GetInterface()->get_Projects(&pDispProjects);
		CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

		// Get the number of projects in the collection
		long projectCount;
		pProjects->get_Count(&projectCount);
		if (index < 1  ||  index > projectCount)
		{
			throw TException(TException::VALUE_OUT_OF_RANGE, "The index [%d] is greater than the project count [%d].",
				index, projectCount);
		}

		// Iterate all the projects.
		CComPtr<IGenericProject> pGenProject;

		// Get the next project
		pProjects->Item(CComVariant(index), &pGenProject);
		CComQIPtr<IGenericProject, &IID_IGenericProject> pProject(pGenProject);
		if (!pProject)
		{
			return;
		}
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
		// Get the current project
		CComPtr<EnvDTE::_Solution> pSolution;
		g_pDTE->get_Solution(&pSolution);
		if (!pSolution)
			return;

		CComPtr<EnvDTE::Projects> pProjects;
		pSolution->get_Projects(&pProjects);
		if (!pProjects)
			return;

		CComPtr<EnvDTE::Project> pProject;
		pProjects->Item(CComVariant(index), &pProject);
#endif WWHIZ_VSNET

		// Get the project name.
		CComBSTR bszStr;
		pProject->get_Name(&bszStr);
		CString projectName = bszStr;
		
		args.Return(projectName);
	}
};


///////////////////////////////////////////////////////////////////////////////
// ProjectFileAdd
///////////////////////////////////////////////////////////////////////////////
class ProjectFileAddCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the project name.
		CString desiredProjectName = args.Get(0);
		
		// Add the document.
		CString filename;
		GetCurrentFilename(filename);
		if (filename.IsEmpty())
		{
			throw TException(TException::NO_ACTIVE_FILE, "There is no active file to add to the project.");
		}
		CComBSTR bstrFilename(filename);
		CComVariant varFilename(bstrFilename);

		ProjectHelper helper;
		helper.FindProject(args.Get(0));

#ifdef WWHIZ_VC6
		// Get DevStudio version.  Only DevStudio 6.0 can use this command.
		CComBSTR bstrVersion;
		ObjModelHelper::GetInterface()->get_Version(&bstrVersion);
		CString strVersion = bstrVersion;
		if (strVersion != "5.0")
		{
			if (FAILED(helper.m_pBuildProject->AddFile(bstrFilename, CComVariant())))
			{
//				throw TException(TException::UNABLE_TO_ADD_FILE_TO_PROJECT, "Unable to add file [%s] to project.", filename);
			}
		}
		else
		{
			AfxMessageBox("Developer Studio 5.0 has no support for adding files "
				"to the project.  Please manually add the file [" + filename + 
				"].");
		}
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
		CComPtr<EnvDTE::Project> pProject = GetActiveProject();
		if (pProject)
		{
			CComPtr<EnvDTE::ProjectItems> pProjectItems;
			pProject->get_ProjectItems(&pProjectItems);

			CComPtr<EnvDTE::ProjectItem> pProjectItem;
			pProjectItems->AddFromFile(CComBSTR(bstrFilename), &pProjectItem);
		}

#endif WWHIZ_VSNET
	}
};


///////////////////////////////////////////////////////////////////////////////
// ProjectName
///////////////////////////////////////////////////////////////////////////////
class ProjectNameCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
#ifdef WWHIZ_VC6
		// Get the current project
		CComPtr<IDispatch> pActiveProject;
		ObjModelHelper::GetInterface()->get_ActiveProject(&pActiveProject);
		if (pActiveProject)
		{
			CComQIPtr<IGenericProject, &IID_IGenericProject> pProject(pActiveProject);
			if (pProject)
			{
				CComBSTR projectName;
				pProject->get_Name(&projectName);
				args.Return(CString(projectName));
			}
		}
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
		CComPtr<EnvDTE::Project> pProject = GetActiveProject();
		if (pProject)
		{
			CComBSTR projectName;
			pProject->get_Name(&projectName);
			args.Return(CString(projectName));
		}
#endif WWHIZ_VSNET
	}
};


///////////////////////////////////////////////////////////////////////////////
// ProjectNew
///////////////////////////////////////////////////////////////////////////////
class ProjectNewCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString projectName = args.Get(0);
		if (projectName.IsEmpty())
		{
			throw TException(TException::PROJECT_NAME_REQUIRED, "A project name is required as parameter 1.");
		}

		CString pathName = args.Get(1);
		if (pathName.IsEmpty())
		{
			throw TException(TException::PATH_REQUIRED, "A path to store the project at is required as parameter 2.");
		}
		if (pathName[pathName.GetLength() - 1] != '\\'  &&
			pathName[pathName.GetLength() - 1] != '/')
			pathName += "\\";

		CString typeStr = args.Get(2);
		if (typeStr.IsEmpty())
		{
			throw TException(TException::PROJECT_TYPE_REQUIRED, "A project type is required as parameter 3.");
		}
		typeStr.MakeLower();

#ifdef WWHIZ_VC6
		// See if this project exists, since AddProject() will
		// overwrite it.
		CString dspName = pathName + projectName + ".dsp";
		bool overwrite = true;
		if (_access(dspName, 00) != -1)
		{
			// The project exists.  Prompt the user!
			CString msg;
			msg.Format("The project [%s] about to be created by the template already exists.  Overwrite?", dspName);
			if (AfxMessageBox(msg, MB_YESNO) == IDNO)
				overwrite = false;
		}
		
		if (overwrite)
		{
			// Get DevStudio version.  Only DevStudio 6.0 can use this command.
			CComBSTR bstrVersion;
			ObjModelHelper::GetInterface()->get_Version(&bstrVersion);
			CString strVersion = bstrVersion;
			if (strVersion != "5.0")
			{
				if (SUCCEEDED(ObjModelHelper::GetInterface()->AddProject(
						CComBSTR(projectName), CComBSTR(pathName),
						CComBSTR(typeStr), VARIANT_TRUE)))
				{
				}
				else
				{
					// Error!!
					throw TException(TException::UNABLE_TO_ADD_NEW_PROJECT, "An unexpected error occurred while adding the project "
						"[%s] to the workspace.", pathName + projectName);
				}
			}
			else
			{
				CString err;
				err.Format("Developer Studio 5.0 has no support for creating new projects.");
				AfxMessageBox(err);
			}
		}
#endif WWHIZ_VC6
	}
};


///////////////////////////////////////////////////////////////////////////////
// ProjectPath
///////////////////////////////////////////////////////////////////////////////
class ProjectPathCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the project name, if any.
		CString projectName = args.Get(0);

#ifdef WWHIZ_VC6
		// Get the current project
		CComPtr<IDispatch> pDispProject;
		if (projectName.IsEmpty())
			ObjModelHelper::GetInterface()->get_ActiveProject(&pDispProject);
		else
			pDispProject = FindProjectByName(projectName);

		// See if project exists.
		if (pDispProject)
		{
			CComQIPtr<IGenericProject, &IID_IGenericProject> pProject(pDispProject);
			if (pProject)
			{
				CComBSTR projectFullName;
				pProject->get_FullName(&projectFullName);
				args.Return(CString(projectFullName));
			}
		}
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
		CComPtr<EnvDTE::Project> pProject = GetActiveProject();
		if (pProject)
		{
			CComBSTR projectName;
			pProject->get_FullName(&projectName);
			args.Return(CString(projectName));
		}
#endif WWHIZ_VSNET
	}
};


///////////////////////////////////////////////////////////////////////////////
// Return
///////////////////////////////////////////////////////////////////////////////
class ReturnCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		curTemplate->SetEntry("__Return", args.Get(0));
	}
};


///////////////////////////////////////////////////////////////////////////////
// Sel
///////////////////////////////////////////////////////////////////////////////
class SelCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		ObjModelHelper objModel;
		objModel.GetActiveDocument();
		args.Return(objModel.GetText());
	}
};


///////////////////////////////////////////////////////////////////////////////
// Set
///////////////////////////////////////////////////////////////////////////////
class SetCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString entry = args.Get(0);
		if (entry.IsEmpty())
		{
			throw TException(TException::MISSING_VARIABLE, "A variable name is required as parameter 1.");
		}

		CString value = args.Get(1);
		CString parent = args.Get(2);

		WWhizTemplate* assignTemplate = curTemplate;
		if (!parent.IsEmpty())
		{
			int index = curTemplate->GetParent().Find(parent);
			if (index != -1)
			{
				WWhizTemplate* temp = curTemplate->GetParent().Get(index);
				if (temp)
					assignTemplate = temp;
			}
		}

		assignTemplate->SetEntry(entry, value);
	}
};


///////////////////////////////////////////////////////////////////////////////
// StrFind
//
// @@(StrFind String FindStr [StartIndex] [1|0 - Case sensitive])@@
// 
// Returns index of string.
///////////////////////////////////////////////////////////////////////////////
class StrFindCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		// Get the string.
		CString string = args.Get(0);
		CString findStr = args.Get(1);

		int startIndex = 0;

		AsciiToInteger(args.Get(2), startIndex);

		bool caseSensitive = true;
		if (args.Get(3) == "0")
			caseSensitive = false;

		int index;
		if (!caseSensitive)
		{
			string.MakeLower();
			findStr.MakeLower();
		}

		index = string.Find(findStr, startIndex);
		args.Return(index);
	}
};


///////////////////////////////////////////////////////////////////////////////
// StrLen
//
// @@(StrLen String)@@
// 
// Returns length of string.
///////////////////////////////////////////////////////////////////////////////
class StrLenCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		args.Return(args.Get(0).GetLength());
	}
};


///////////////////////////////////////////////////////////////////////////////
// StrLower
//
// @@(StrLower String [Index])@@
// 
// 
///////////////////////////////////////////////////////////////////////////////
class StrLowerCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString str = args.Get(0);

		int startIndex = 0;
		if (AsciiToInteger(args.Get(1), startIndex))
		{
			str.SetAt(startIndex, tolower(str[startIndex]));
		}
		else
		{
			str.MakeLower();
		}

		args.Return(str);
	}
};


///////////////////////////////////////////////////////////////////////////////
// StrSub
//
// @@(StrSub String StartIndex Count)@@
// 
// Returns a substring of String.
///////////////////////////////////////////////////////////////////////////////
class StrSubCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString str = args.Get(0);

		int startIndex;
		int count;

		if (!AsciiToInteger(args.Get(1), startIndex))
		{
			throw TException(TException::MISSING_VARIABLE, "A numeric start index is required as parameter 2.");
		}

		if (!AsciiToInteger(args.Get(2), count))
		{
			args.Return(str.Mid(startIndex));
		}
		else
		{
			args.Return(str.Mid(startIndex, count));
		}
	}
};


///////////////////////////////////////////////////////////////////////////////
// StrUpper
//
// @@(StrUpper String [StartIndex])@@
// 
// 
///////////////////////////////////////////////////////////////////////////////
class StrUpperCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString str = args.Get(0);

		int startIndex = 0;
		if (AsciiToInteger(args.Get(1), startIndex))
		{
			str.SetAt(startIndex, toupper(str[startIndex]));
		}
		else
		{
			str.MakeUpper();
		}

		args.Return(str);
	}
};


///////////////////////////////////////////////////////////////////////////////
// String
///////////////////////////////////////////////////////////////////////////////
class StringCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		args.Return(args.Get(0));
	}
};


///////////////////////////////////////////////////////////////////////////////
// Sub
///////////////////////////////////////////////////////////////////////////////
class SubCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString varName = args.Get(0);
		CString value = curTemplate->GetEntry(varName);
		CString addAmount = args.Get(1);
		int valueInt;
		int addAmountInt;

		if (AsciiToInteger(value, valueInt)  &&  AsciiToInteger(addAmount, addAmountInt))
		{
			valueInt -= addAmountInt;
			value.Format("%d", valueInt);
			curTemplate->SetEntry(varName, value);
		}
	}
};


///////////////////////////////////////////////////////////////////////////////
// Time
///////////////////////////////////////////////////////////////////////////////
class TimeCommand : public WWhizTemplateCommand
{
public:
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args)
	{
		CString format = args.Get(0);
		LPCTSTR formatPtr = format.IsEmpty() ? NULL : (LPCTSTR)format;

		int timeLen =
		   ::GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, formatPtr, NULL, 0);
		CString out;
		LPTSTR outBuf = out.GetBuffer(timeLen);
		::GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, formatPtr, outBuf, timeLen);
		out.ReleaseBuffer();
		args.Return(out);
	}
};


// Regular commands.
static AddCommand						s_addCommand;
static CallCommand						s_callCommand;
static CaretCommand						s_caretCommand;
static ConfigAddFileSettingsCommand		s_configAddFileSettingsCommand;
static ConfigAddToolSettingsCommand		s_configAddToolSettingsCommand;
static ConfigCountCommand				s_configCountCommand;
static ConfigGetCommand					s_configGetCommand;
static ConfigRemoveFileSettingsCommand	s_configRemoveFileSettingsCommand;
static ConfigRemoveToolSettingsCommand	s_configRemoveToolSettingsCommand;
static DateCommand						s_dateCommand;
static DialogCommand					s_dialogCommand;
static EnvCommand						s_envCommand;
static ExecuteCommandCommand			s_executeCommandCommand;
static FileExtCommand					s_fileExtCommand;
static FileNewCommand					s_fileNewCommand;
static FileNewTextCommand				s_fileNewTextCommand;
static FilePathCommand					s_filePathCommand;
static FileSaveCommand					s_fileSaveCommand;
static FileTitleCommand					s_fileTitleCommand;
static FindCommand						s_findCommand;
static GetPosCommand					s_getPosCommand;
static GotoCommand						s_gotoCommand;
static GUIDCommand						s_guidCommand;
static LabelCommand						s_labelCommand;
static MakeDirCommand					s_makeDirCommand;
static MoveToCommand					s_moveToCommand;
static MessageBoxCommand				s_messageBoxCommand;
static ProjectCountCommand				s_projectCountCommand;
static ProjectFileAddCommand			s_projectFileAddCommand;
static ProjectGetCommand				s_projectGetCommand;
static ProjectNameCommand				s_projectNameCommand;
static ProjectNewCommand				s_projectNewCommand;
static ProjectPathCommand				s_projectPathCommand;
static ReturnCommand					s_returnCommand;
static SelCommand						s_selCommand;
static SetCommand						s_setCommand;
static StrFindCommand					s_strFindCommand;
static StrLenCommand					s_strLenCommand;
static StrLowerCommand					s_strLowerCommand;
static StrSubCommand					s_strSubCommand;
static StrUpperCommand					s_strUpperCommand;
static StringCommand					s_stringCommand;
static SubCommand						s_subCommand;
static TimeCommand						s_timeCommand;


/////////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport)
#ifdef WWHIZ_VC6
bool WWhizTemplateRegister(WWhizTemplateManager* templateManager,
		IApplication* pApplication)
{
	g_pApplication = pApplication;
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
bool WWhizTemplateRegister(WWhizInterface* wwhizInterface,
						   WWhizTemplateManager* templateManager,
						   void* pDTE)
{
	g_pDTE = (EnvDTE::_DTE*)pDTE;
#endif WWHIZ_VSNET
	g_templateManager = templateManager;

	// Regular commands
	g_templateManager->RegisterCommand("Add",						s_addCommand);
	g_templateManager->RegisterCommand("Call",						s_callCommand);
	g_templateManager->RegisterCommand("Caret",						s_caretCommand);
	g_templateManager->RegisterCommand("ConfigAddFileSettings",		s_configAddFileSettingsCommand);
	g_templateManager->RegisterCommand("ConfigAddToolSettings",		s_configAddToolSettingsCommand);
	g_templateManager->RegisterCommand("ConfigCount",				s_configCountCommand);
	g_templateManager->RegisterCommand("ConfigGet",					s_configGetCommand);
	g_templateManager->RegisterCommand("ConfigRemoveFileSettings",	s_configRemoveFileSettingsCommand);
	g_templateManager->RegisterCommand("ConfigRemoveToolSettings",	s_configRemoveToolSettingsCommand);
	g_templateManager->RegisterCommand("Date",						s_dateCommand);
	g_templateManager->RegisterCommand("Dialog",					s_dialogCommand);
	g_templateManager->RegisterCommand("Env",						s_envCommand);
	g_templateManager->RegisterCommand("ExecuteCommand",			s_executeCommandCommand);
	g_templateManager->RegisterCommand("FileExt",					s_fileExtCommand);
	g_templateManager->RegisterCommand("FileNew",					s_fileNewCommand);
	g_templateManager->RegisterCommand("FileNewText",				s_fileNewTextCommand);
	g_templateManager->RegisterCommand("FilePath",					s_filePathCommand);
	g_templateManager->RegisterCommand("FileSave",					s_fileSaveCommand);
	g_templateManager->RegisterCommand("FileTitle",					s_fileTitleCommand);
	g_templateManager->RegisterCommand("Find",						s_findCommand);
	g_templateManager->RegisterCommand("GetPos",					s_getPosCommand);
	g_templateManager->RegisterCommand("Goto",						s_gotoCommand);
	g_templateManager->RegisterCommand("GUID",						s_guidCommand);
	g_templateManager->RegisterCommand("Label",						s_labelCommand);
	g_templateManager->RegisterCommand("MakeDir",					s_makeDirCommand);
	g_templateManager->RegisterCommand("MoveTo",					s_moveToCommand);
	g_templateManager->RegisterCommand("MessageBox",				s_messageBoxCommand);
	g_templateManager->RegisterCommand("ProjectCount",				s_projectCountCommand);
	g_templateManager->RegisterCommand("ProjectFileAdd",			s_projectFileAddCommand);
	g_templateManager->RegisterCommand("ProjectGet",				s_projectGetCommand);
	g_templateManager->RegisterCommand("ProjectName",				s_projectNameCommand);
	g_templateManager->RegisterCommand("ProjectNew",				s_projectNewCommand);
	g_templateManager->RegisterCommand("ProjectPath",				s_projectPathCommand);
	g_templateManager->RegisterCommand("Return",					s_returnCommand);
	g_templateManager->RegisterCommand("Sel",						s_selCommand);
	g_templateManager->RegisterCommand("Set",						s_setCommand);
	g_templateManager->RegisterCommand("StrFind",					s_strFindCommand);
	g_templateManager->RegisterCommand("StrLen",					s_strLenCommand);
	g_templateManager->RegisterCommand("StrLower",					s_strLowerCommand);
	g_templateManager->RegisterCommand("StrSub",					s_strSubCommand);
	g_templateManager->RegisterCommand("StrUpper",					s_strUpperCommand);
	g_templateManager->RegisterCommand("String",					s_stringCommand);
	g_templateManager->RegisterCommand("Sub",						s_subCommand);
	g_templateManager->RegisterCommand("Time",						s_timeCommand);

	return true;
}


extern "C" __declspec(dllexport)
void WWhizTemplateDestroy()
{
#ifdef WWHIZ_VSNET
	g_pDTE = NULL;
#endif WWHIZ_VSNET
}

						   
