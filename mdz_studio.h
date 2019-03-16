#ifndef MDZ_STUDIO_H
#define MDZ_STUDIO_H

#include "mdz_wtk.h"
#include "MdzSignalThread.h"

class ShortCutCtrl : public MdzPanel
{
public:
	void SetValue(DWORD iValue);
	DWORD GetValue();
	ShortCutCtrl();
	MdzComboBox cbChanger;
	MdzComboBox cbChar;

protected:
	virtual void OnSize(int width, int height);
private:
	void InitChar(BOOL bFull);
	void cbChanger_OnSelChange();
};

class Prop : public MdzObject
{
	int type;
	MdzString name;
	MdzString value;
	MdzString label;
	MdzCtrl* pCtrl;
public:
	enum {TEXT, DIGIT, BOOL, CHOICE, COLOR, FONT, BITMAP, SHORTCUT};

	MdzString GetName() {return name;}


	Prop(int type, MdzString name, MdzString label, MdzString value);

	void Create(MdzLayout& layout);

	MdzString GetValue();

	void SetValue(MdzString value);

private:
	ShortCutCtrl* AddShortCut(MdzLayout& layout, MdzString label, MdzString values);
	MdzCtrl* AddLabelledBitmap(MdzLayout& layout, MdzString label, MdzString bmpId);

	MdzCtrl* AddLabelledColor(MdzLayout& layout, MdzString label, MdzString color);

	MdzCtrl* AddLabelledFont(MdzLayout& layout, MdzString label, MdzString font);

	void BtnFont_OnClick(LPVOID pArg);
	
	void BtnColor_OnClick(LPVOID pArg);

	void BtnBitmap_OnClick(LPVOID pArg);


	MdzCtrl* AddLabelledCheckBox(MdzLayout& layout, MdzString label, MdzString isChecked);

	MdzTextField* AddLabelledTextField(MdzLayout& layout, MdzString label, MdzString value);

	void AddLabelledCtrl(MdzLayout& layout, MdzString label, MdzCtrl& ctrl);

	MdzComboBox* AddLabelledComboBox(MdzLayout& layout, MdzString label, MdzString values);
};

class Form
{
	MdzVectorT<Prop> props;
	MdzLayout layout;
public:
	void SetValue(MdzString name, MdzString value);
	void AddBorder();
	Form(MdzContainer* pCtrl);
	
	void AddProp(int type, MdzString name, MdzString value);

	void AddProp(int type, MdzString name, MdzString label, MdzString value);



	MdzString GetValue(MdzString name);

	void Save(MdzXmlWriter& writer);

	void Load(MdzXmlElement* pElt);

};

class OutputStream
{
private:
	FILE* pFile;
	int iLine;
public:
	OutputStream();
	~OutputStream();

	void Close();

	BOOL Open(char* fileName);

	void Write(char* fmt, ...);

	void WriteLn(char* fmt, ...);

	void WriteLn();

	int GetLineIndex() {return iLine;}
};

class MyDialog : public MdzDialog
{
private:
	MdzButton btnOK;
	MdzButton btnCancel;
	MdzCtrl* pFocusCtrl;
public:
	void SetFocusCtrl(MdzCtrl& ctrl);
	MyDialog();
protected:
	void AddOKCancelButtons(MdzLayout& layout, int spacing = 10);
	void AddLabelledCtrl(MdzLayout& layout, MdzString label, MdzCtrl& ctrl);
	
	virtual BOOL Check() {return TRUE;}

private:
	void btnOK_OnClick();

	void btnCancel_OnClick();

	void OnFocus(UINT bGotFocus);
};

//////////////////////////////////////////////////////////////

class MyNode : public MdzTreeNode
{
protected:
	MdzPanel panel;
	MdzMenu ctxMenu;
	MdzLabel label;
	Form form;
private:
	BOOL bIsRemovable;
public:
	DEFINE_STD_METHODS(MyNode, MdzTreeNode);
	MdzString GetParentClassName(); 
	MdzString GetClassName() {return GetClass();}

	BOOL IsRemovable() {return bIsRemovable;}

	virtual BOOL CanMove() {return FALSE;}
	virtual BOOL IsCopyable() {return TRUE;}


	virtual void Save(MdzXmlWriter& writer);

	void SetRemovable(BOOL bIsRemovable) {this->bIsRemovable = bIsRemovable;}

	MyNode();

	virtual void Load(MdzXmlElement* pElt);

	void LoadProps(MdzXmlElement* pElt);

	void AddMethodCall(OutputStream& stream, char* methodName, char* methodParameters);

	virtual void EncodeHead(OutputStream& stream);

	virtual void EncodeConstructor(OutputStream& stream); 

	virtual void EncodeImp(OutputStream& stream); 

	virtual MdzMenu* GetCtxMenu();
	virtual void InitProps();

	virtual void Activate(MdzCtrl* pCtrl);

	virtual void Unselect();

	void AddNoCheck(MyNode* pNode);

	virtual BOOL AddNode(MyNode* pNode, BOOL bShowWarning = TRUE);


	MdzTreeNode* IsNameExiting(MdzString strName);


	virtual MdzString GetRealName();

	MdzString GetFatherClass();

	virtual MdzString GetLabel() {return "";}

	void AddMenuItem(MdzString caption, MdzCbk1* pFunc = NULL);

	void AddMenuSeparator();

};




class LayoutNode : public MyNode
{
public:
	DEFINE_STD_METHODS(LayoutNode, MyNode);
	
	MdzString GetClassName() {return "LayoutNode";}

	static LayoutNode* Create(char* name);

	void InitProps();
	virtual BOOL IsCopyable() {return FALSE;}

	void AddControl(char* ctrlName);

	void addControl_OnClick(void* pArg);


	virtual void EncodeConstructor(OutputStream& stream);

private:
	void AddControl(char* ctrlName, UINT iResId);
	void addBorder_OnClick();
};
/////////////////////////////////////////////////////////////////////
class DlgClass : public MyDialog
{
	MdzTextField txtClass;
	MdzTextField txtBaseClass;
public:
	MdzString GetName() {return txtClass.GetText();}
	MdzString GetBaseClass() {return txtBaseClass.GetText();}

	DlgClass();
};

class DlgVarChoice : public MdzDialog
{
	MdzTabCtrl tabCtrl;
	MdzListBox list[6];
public:
	DlgVarChoice();

	void list_OnDblClick();

	MdzString GetSelType();
};

class DlgVariable : public MyDialog
{
	MdzTextField txtName;
	MdzTextField txtType;
	MdzTextField txtArraySize;
	MdzButton button;
	MdzCheckBox chkPointer;
	MdzCheckBox chkArray;
public:
	MdzString GetName();

	int GetArraySize();

	MdzString GetType(); 

	DlgVariable();

	BOOL Check();

	void chkArray_OnClick();

	void button_OnClick();
};

class DlgRegister : public MdzDialog
{
	MdzButton btnBuy;
	MdzPanel panel;
public:
	MdzString strPrice;
	MdzString strProductId;
	DlgRegister();
protected:
	void panel_OnPaint(MdzGraphic& graphic);
private:
	void btnBuy_OnClick();
};

class DlgResource : public MyDialog
{
	MdzListBox list;
	MdzButton btnImport;
	MdzButton btnRename;
	MdzButton btnRemove;
	MdzPicture picture;
	int iLastIndex;
	MdzString strProjectPath;

public:
	BOOL GetBitmapId(MdzString& strInitValue);

	void list2_OnDblClick(LPVOID pArg);

	void Init();

	void WriteHeader(OutputStream& stream);

	void WriteScript(FILE* pFile);

	void Load(MdzString strProjectPath, MdzXmlElement* pElt);

	void Save(MdzXmlWriter& writer);

	DlgResource();

	void EnableButtons(BOOL bIsEnabled);

	void btnRename_OnClick();

	void btnRemove_OnClick();

	void list_OnSelChange();

	void btnImport_OnClick();
};

class DlgMethod : public MyDialog
{
	MdzTextField txtName;
	MdzTextField txtProto;
	MdzTextField txtRetValue;
	MdzCheckBox chkVirtual;
	MdzCheckBox chkStatic;

public:
	MdzString GetName() {return txtName.GetText();}
	MdzString GetProto() {return txtProto.GetText();}
	MdzString GetRetValue(); 

	int GetType();

	DlgMethod();

	void chkVirtual_OnClick();

	void chkStatic_OnClick();
	
	BOOL Check();
};

class DlgProject : public MyDialog
{
	MdzDriveComboBox cbDrive;
	MdzDirListBox lstDirectory;
	MdzTextField txtName;
	MdzLabel lblPath;
public:
	DlgProject();

	MdzString GetPath() {return lstDirectory.GetPath();}
	MdzString GetName() {return txtName.GetText();}

	BOOL Check();

	void cbDrive_OnSelChange();
private:
	void lstDirectory_OnDblClick();
};

class DlgCode : public MdzDialog
{
	MdzTextArea textArea;
public:
	DlgCode(char* fileName);


};

////////////////////////////////////////////////////////////////////

class VarNode;

class ClassNode : public MyNode
{
protected:
	virtual int OnCompareChild(MdzTreeNode *pNode1, MdzTreeNode *pNode2);
	MdzString baseClass;
public:
	MdzString GetBaseClass();
	void GetVarList(MdzString strPrefix, MdzStrVector& vector);
	DEFINE_STD_METHODS(ClassNode, MyNode);
	ClassNode();
	static ClassNode* Create(MdzString name, MdzString baseClass);

	void addInitDoneMethods();;
	
	VarNode* GetVarNode(MdzString strName);

	void InitProps();

	void Save(MdzXmlWriter& writer);

	void Load(MdzXmlElement* pElt);

	MdzString GetLabel();

	virtual void EncodeHead(OutputStream& stream);

	virtual void EncodeImp(OutputStream& stream); 

	void addMethod_OnClick();

	void addVariable_OnClick();

};

class MethodParameter : public MdzObject
{
public:
	MdzString strName;
	MdzString strType;
};

class MethodNode : public MyNode
{
protected:
	MdzString proto;
	MdzString retValue;
	MdzTextArea code;
	int type;
	int iStartLine;
	int iEndLine;
	MdzVectorT<MethodParameter> parameters;


	void code_OnChar(UINT iChar);

	MdzString GetCode();

	void Decode(MdzString strCode);

public:
	void DisplayVarList();
	MethodNode();
	~MethodNode();
	DEFINE_STD_METHODS(MethodNode, MyNode)

	int GetStartLine() {return iStartLine;}
	int GetEndLine() {return iEndLine;}

	virtual void Activate(MdzCtrl* pCtrl);

	MdzTextArea* GetTextArea() {return &code;}

	void SetCode(char* strCode);

	static MethodNode* Create(MdzString name, MdzString proto, MdzString retValue, int type = 0);

	void InitProps();

	void Load(MdzXmlElement* pElt);

	void Save(MdzXmlWriter& writer);

	MdzString GetLabel();

	void panel_OnSize(int width, int height);

	virtual void EncodeHead(OutputStream& stream);

	virtual void EncodeImp(OutputStream& stream);
private:
	BOOL IsInQuote(int iIndex);
	BOOL IsGoodChar(UCHAR ch);
	void code_OnLButtonDblClick(int x, int y);
	void code_OnEnter();
	MdzString GetParameterType(MdzString strName);
	void GetParameters();
	MdzString FindLocalVariable(MdzString strCode, MdzString strVarName, BOOL& bIsPointer);
	MdzString GetKeyWord(MdzString strCode, int iIndex);
};

class CallbackNode : public MethodNode
{
public:
	CallbackNode();
	DEFINE_STD_METHODS(CallbackNode, MethodNode);
	static CallbackNode* Create(MdzString name, MdzString proto, MdzString retValue);

	virtual void EncodeConstructor(OutputStream& stream);

	int GetMacroNb();

};

class VarNode : public MyNode
{
protected:
	MdzString type;
	int size;
public:
	VarNode();
	DEFINE_STD_METHODS(VarNode, MyNode)
	void AddCallbackNode(MdzString name, MdzString proto, MdzString retValue);

	static VarNode* Create(MdzString name, MdzString type, int size = 0);

	void Load(MdzXmlElement* pElt);

	void Save(MdzXmlWriter& writer);

	MdzString GetLabel();

	MdzString GetType() {return type;}

	virtual void EncodeHead(OutputStream& stream);

};

class TimerNode : public VarNode
{
public:
	DEFINE_STD_METHODS(TimerNode, VarNode)

	TimerNode() {type = "MdzTimer";}


	virtual void EncodeConstructor(OutputStream& stream);
	void InitProps();

	void addOnTimeout_OnClick();
};

class SocketClientNode : public VarNode
{
public:
	DEFINE_STD_METHODS(SocketClientNode, VarNode)

	SocketClientNode() {type = "MdzSocketClient";}
protected:
	virtual void InitProps();
private:
	void addOnConnectionClosed_OnClick();
	void addOnDataReceived_OnClick();
};

class SocketServerNode : public VarNode
{
public:
	DEFINE_STD_METHODS(SocketServerNode, VarNode)
	SocketServerNode() {type = "MdzSocketServer";}
protected:
	virtual void InitProps();
private:
	void addOnIncomingConnection_OnClick();
};


class MenuItemNode : public VarNode
{
public:	
	MenuItemNode();
	DEFINE_STD_METHODS(MenuItemNode, VarNode)
	BOOL CanMove(); 

	static MenuItemNode* Create(char* name);

	virtual void EncodeConstructor(OutputStream& stream);
	void InitProps();

	void addOnClick_OnClick();


};


class MenuSeparatorNode : public MyNode
{
public:
	DEFINE_STD_METHODS(MenuSeparatorNode, MyNode)
	BOOL CanMove() {return TRUE;}
	virtual BOOL IsCopyable() {return FALSE;}

	MenuSeparatorNode();

	virtual void EncodeConstructor(OutputStream& stream);
};

class LayoutBorderNode : public MyNode
{
public:
	virtual void InitProps();
	virtual void EncodeConstructor(OutputStream& stream);
	LayoutBorderNode();
	DEFINE_STD_METHODS(LayoutBorderNode, MyNode)
	BOOL CanMove() {return TRUE;}
	virtual BOOL IsCopyable() {return FALSE;}

};

class MenuNode : public VarNode
{
public:	
	MenuNode();
	DEFINE_STD_METHODS(MenuNode, VarNode)
	BOOL CanMove(); 

	static MenuNode* Create(char* name);

	virtual void EncodeConstructor(OutputStream& stream);
	void InitProps();

	void addOnClick_OnClick();
	
	void addSeparator_OnClick();

	void addMenu_OnClick();

	void addMenuItem_OnClick();

};


class MenuBarNode : public VarNode
{
public:
	DEFINE_STD_METHODS(MenuBarNode, VarNode)

	MenuBarNode();

	virtual void EncodeConstructor(OutputStream& stream);
	void InitProps();

	void addMenu_OnClick();
};


class CtrlNode : public VarNode
{
public:
	virtual void Activate(MdzCtrl* pCtrl);
	DEFINE_STD_METHODS(CtrlNode, VarNode)

	BOOL CanMove(); 

	virtual void EncodeConstructor(OutputStream& stream);

	void InitProps();
	
	MdzString GetWidth() {return form.GetValue("Width");}
	MdzString GetHeight() {return form.GetValue("Height");}
	MdzString GetLayoutMode() {return form.GetValue("LayoutMode");}
	MdzString GetLayoutSpacing() {return form.GetValue("LayoutSpacing");}
private:
	void panel_OnSize(int width, int height);
	MdzPanel propertiesPane;
protected:
	virtual MdzString GetLabelWidth();
	virtual MdzString GetLabelText();
};

class LabelledCtrlNode : public CtrlNode
{
public:
	DEFINE_STD_METHODS(LabelledCtrlNode, CtrlNode)
protected:
	virtual MdzString GetLabelWidth();
	virtual void InitProps();
	virtual MdzString GetLabelText();
};


class SliderNode : public CtrlNode
{
public:
	virtual void EncodeConstructor(OutputStream &stream);
	virtual void InitProps();
	SliderNode();
	DEFINE_STD_METHODS(SliderNode, CtrlNode)
private:
	void addOnSelChange_OnClick();
};

class TabCtrlNode : public CtrlNode
{
public:
	TabCtrlNode();
	DEFINE_STD_METHODS(TabCtrlNode, CtrlNode)
protected:
	virtual void InitProps();
private:
	void addTab_OnClick();
};

class TabPanelNode : public VarNode
{
public:
	static TabPanelNode* Create(char* name);
	TabPanelNode() {type = "MdzPanel";}
	DEFINE_STD_METHODS(TabPanelNode, VarNode)

protected:
	virtual void EncodeConstructor(OutputStream& stream);
	virtual void InitProps();
private:
	void addOnSize_OnClick();
	void addLayout_OnClick();
};

class TreeCtrlNode : public CtrlNode
{
public:
	TreeCtrlNode();
	DEFINE_STD_METHODS(TreeCtrlNode, CtrlNode)
protected:
	virtual void InitProps();
private:
	void addOnRightClick_OnClick();
	void addOnSelChange_OnClick();
};

class TableNode : public CtrlNode
{
public:
	TableNode();
	DEFINE_STD_METHODS(TableNode, CtrlNode)
};

class ButtonNode : public CtrlNode
{
public:

	DEFINE_STD_METHODS(ButtonNode, CtrlNode)
	
	ButtonNode();


	void InitProps();

	void addOnClick_OnClick();

	virtual void EncodeConstructor(OutputStream& stream);
};

class ListBoxNode : public CtrlNode
{
public:

	DEFINE_STD_METHODS(ListBoxNode, CtrlNode)
	
	ListBoxNode();

	void InitProps();

	void addOnSelChange_OnClick();

	void addOnDblClick_OnClick();

	virtual void EncodeConstructor(OutputStream& stream);
};


class ComboBoxNode : public LabelledCtrlNode
{
public:

	DEFINE_STD_METHODS(ComboBoxNode, LabelledCtrlNode)
	
	ComboBoxNode();


	void InitProps();

	void addOnSelChange_OnClick();


	virtual void EncodeConstructor(OutputStream& stream);
};


class TextFieldNode : public LabelledCtrlNode
{
public:

	DEFINE_STD_METHODS(TextFieldNode, LabelledCtrlNode)
	
	TextFieldNode();


	void InitProps();


	virtual void EncodeConstructor(OutputStream& stream);

};


class TextAreaNode : public CtrlNode
{
public:

	DEFINE_STD_METHODS(TextAreaNode, CtrlNode)
	
	TextAreaNode();



	void InitProps();



	virtual void EncodeConstructor(OutputStream& stream);

};


class PanelNode : public CtrlNode
{
public:

	DEFINE_STD_METHODS(PanelNode, CtrlNode)
	
	PanelNode();



	void InitProps();


	void addLayout_OnClick();


	void addOnSize_OnClick();


	void addOnKeyDown_OnClick();


	void addOnKeyUp_OnClick();


	void addOnPaint_OnClick();


	void addOnMouseMove_OnClick();


	void addOnMouseDrag_OnClick();


	void addOnLeftButtonDown_OnClick();


	void addOnLeftButtonUp_OnClick();


	void addOnRightButtonDown_OnClick();


	void addOnRightButtonUp_OnClick();
	

	virtual void EncodeConstructor(OutputStream& stream);

protected:
	virtual int OnCompareChild(MdzTreeNode *pNode1, MdzTreeNode *pNode2);
};


class LabelNode : public CtrlNode
{
public:

	DEFINE_STD_METHODS(LabelNode, CtrlNode)
	
	LabelNode();



	void InitProps();



	virtual void EncodeConstructor(OutputStream& stream);

};


class ToolComboNode : public VarNode
{
public:

	static ToolComboNode* Create(char* name);
	

	DEFINE_STD_METHODS(ToolComboNode, VarNode)
	
	ToolComboNode();
	

	BOOL CanMove(); 
	
	
	virtual void EncodeConstructor(OutputStream& stream);
	
	void addOnSelChange_OnClick();


	void InitProps();
	
};


class ToolButtonNode : public VarNode
{
public:

	static ToolButtonNode* Create(char* name);
	

	DEFINE_STD_METHODS(ToolButtonNode, VarNode)
	
	ToolButtonNode();
	

	BOOL CanMove(); 
	
	
	virtual void EncodeConstructor(OutputStream& stream);
	

	void addOnClick_OnClick();
	


	void InitProps();
	
};


class ToolBarSeparatorNode : public MyNode
{
public:
	DEFINE_STD_METHODS(ToolBarSeparatorNode, MyNode)
	BOOL CanMove() {return TRUE;}
	virtual BOOL IsCopyable() {return FALSE;}

	ToolBarSeparatorNode();
	

	virtual void EncodeConstructor(OutputStream& stream);
	
};


class ToolBarNode : public VarNode
{
public:

	static ToolBarNode* Create(char* name);
	

	DEFINE_STD_METHODS(ToolBarNode, VarNode)
	
	ToolBarNode();
	

	virtual void EncodeConstructor(OutputStream& stream);
	

	void addButton_OnClick();
	

	void addComboBox_OnClick();
	

	void addSeparator_OnClick();
	

	void InitProps();
	
};


class DockBarNode : public VarNode
{
public:

	DEFINE_STD_METHODS(DockBarNode, VarNode)
	
	DockBarNode();
	

	virtual void EncodeConstructor(OutputStream& stream);
	

	void addToolBar_OnClick();
	

	void InitProps();
	
};


class CheckBoxNode : public CtrlNode
{
public:

	DEFINE_STD_METHODS(CheckBoxNode, CtrlNode)
	
	CheckBoxNode();
	

	void addOnClick_OnClick();
	

	void InitProps();
	


	virtual void EncodeConstructor(OutputStream& stream);
	
};


class RadioButtonNode : public CtrlNode
{
public:
	void EncodeConstructor(OutputStream& stream);
	void InitProps();
	RadioButtonNode();

	DEFINE_STD_METHODS(RadioButtonNode, CtrlNode)
private:
	void addOnClick_OnClick();
};



class FrameNode : public ClassNode
{
public:

	DEFINE_STD_METHODS(FrameNode, ClassNode)

	FrameNode();
	

	static FrameNode* Create();
	


	void InitProps();
	

	void addDockBar_OnClick();
	

	void addMenuBar_OnClick();
	

	void addLayout_OnClick();
	
	void addOnSize_OnClick();
	

	void addOnClose_OnClick();
	

	virtual void EncodeConstructor(OutputStream& stream);
	

	virtual void EncodeImp(OutputStream& stream);
	
};

class DialogNode : public ClassNode
{
public:

	DEFINE_STD_METHODS(DialogNode, ClassNode)

	DialogNode();
	

	static DialogNode* Create(LPSTR strName);
	

	void InitProps();
	

	void addOnFocus_OnClick();
	

	void addLayout_OnClick();
	


	virtual void EncodeConstructor(OutputStream& stream);
	

};


class RootNode : public MyNode
{
public:
	static RootNode* Create();
	
	RootNode();
	

	DEFINE_STD_METHODS(RootNode, MyNode)

	virtual BOOL IsCopyable() {return FALSE;}

	void addDialog_OnClick();	
	void addClass_OnClick();
	
};

class MethodInfo : public MdzObject
{
public:
	MdzString strName;
	UINT iType;
};

class DatabaseRecord : public MdzObject
{
public:
	~DatabaseRecord();
	void AddMethod(LPSTR strName, UINT iType);
	MdzString strName;
	MdzString strBaseName;
	MdzVectorT<MethodInfo> methods;
};

class Database
{
	MdzVectorT<DatabaseRecord> records;
public:
	enum {METHOD_NORMAL = 1, METHOD_STATIC = 2, METHOD_VIRTUAL = 4};
	void GetMethodList(MdzString strClass, MdzStrVector& vector, UINT iType, BOOL bRecursive);
	BOOL LoadDatabase(MdzString strPath);
};

////////////////////////////////////////////////////////////////////////////


class MaFenetre : public MdzFrame
{
	MdzTreeCtrl treeCtrl;
	MyNode* pSelNode;
	MdzMenuBar menuBar;

	MdzMenu fileMenu;
	MdzMenuItem saveItem;
	MdzMenuItem openItem;
	MdzMenuItem newItem;
	MdzMenuItem closeItem;

	MdzMenu projectMenu;
	MdzMenuItem buildItem;
	MdzMenuItem runItem;
	MdzMenuItem resourceItem;

	MdzMenu editMenu;
	MdzMenuItem copyItem;
	MdzMenuItem cutItem;
	MdzMenuItem pasteItem;
	MdzMenuItem findItem;

	MdzMenu helpMenu;
	MdzMenuItem keyItem;
	MdzMenuItem registerItem;

	MdzString strProjectName;
	MdzString strPathName;
	MdzListBox buildList;
	MdzPanel propPanel;
	MdzHorzDividerPanel divPanel;
	MdzPanel leftPanel;
	
	MdzDockBar dockBar;
	MdzToolBar toolBar;
	MdzToolBar toolBar2;
	MdzToolBar toolBar3;
	

	MdzToolButton newBtn;
	MdzToolButton openBtn;	
	MdzToolButton saveBtn;
	MdzToolButton buildBtn;
	MdzToolButton runBtn;
	MdzToolButton stopBtn;


	MdzToolButton deleteBtn;
	MdzToolButton moveupBtn;
	MdzToolButton movedownBtn;

	MdzToolButton cutBtn;
	MdzToolButton pasteBtn;
	MdzToolButton copyBtn;
	MdzToolButton findBtn;

	DlgResource dlgResource;
	MdzTabCtrl tabCtrl;
	MdzTextArea debugText;
	MdzThread debugThread;
	HANDLE hProcess;
	BOOL bIsRegistered;


	void listbox_OnDblClick();


	BOOL CreateChildProcess(LPSTR strCmdLine, HANDLE& hProcess, HANDLE& hPipe);

public:
	MdzString GetProductID();
	ClassNode* GetClassNode(MdzString strName);


	void methodNode_OnFocus(UINT bGotFocus);


	void methodNode_OnSelChange();


	BOOL GetBitmapId(MdzString& strInitValue);


	void Init(BOOL bEnabled);


	void OnHelp();
	

	MaFenetre();


	void InitMenu();


	void apiItem_OnClick();


	void aboutItem_OnClick();


	void moveupBtn_OnClick();


	void movedownBtn_OnClick();
	

	void resourceItem_OnClick();
	

	void deleteBtn_OnClick();
	

	void cutItem_OnClick(); 

	
	void copyItem_OnClick(); 

	
	void pasteItem_OnClick(); 



	void editMenu_OnClick();




	void AddToolButton(MdzToolBar& toolbar, UINT iBmpIdx, MdzCbk1 *pFunc = NULL);


	void AddToolButton(MdzToolBar& toolbar, MdzBitmap bmp, MdzCbk1 *pFunc = NULL);


	void SetDebug(BOOL bEnabled);


	void stopItem_OnClick();


	void runItem_OnClick();


	void debugThread_OnRun();


	BOOL OnClose();


	void closeItem_OnClick();


	void newItem_OnClick();


	void openItem_OnClick();


	void GetMethodList(MdzString strClass, MdzStrVector& vector, BOOL bRecursive = FALSE);


	void LoadDatabase();



	void OnCreate();


	void saveItem_OnClick();


	void SaveNode(MdzXmlWriter& writer, MyNode* pNode);


	BOOL LoadNode(MdzXmlElement* pElt, MyNode* pParentNode);


	void SaveProject(BOOL bAskConfirm = TRUE);


	BOOL LoadProject(MdzString projectName);
	
	MethodNode* FindMethodNode(int iLineNumber);

	void buildItem_OnClick();
	

	void BuildProject();


	void treeCtrl_OnSelChange();


	void treeCtrl_OnRightClick(int x, int y);

	void panel_OnSize(int width, int height);


private:
	void OnDisplayVarList();
	void leftPanel_OnSize(int width, int height);
	void propPanel_OnSize(int width, int height);
	void exitItem_OnClick();
	MdzString strProductId;
	void helpMenu_OnClick();
	BOOL CheckKey(MdzString strKey, MdzString strProductId);
	void keyItem_OnClick();
	void registerItem_OnClick();
	void GetMacAddress(MdzStrVector& vector);
	void findItem_OnClick();
protected:
	virtual void OnSize(MdzRect& rc);
};


#endif //MDZ_STUDIO_H





