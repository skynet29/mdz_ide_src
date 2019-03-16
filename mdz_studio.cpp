#include "mdz_studio.h"
#include "resource.h"
#include "MdzSignalThread.h"

#undef DEV
//#define DEV

#define TABHEIGHT 150
#define TREEWIDTH 200
#define PROP_INTERSPACE 2

Database dbClass;

#define APPNAME	"MDZ IDE"
#define VERSION	"4.1"
#define PRICE	"15"

///////////////////////////////////////////////////////////
MDZ_DECLARE_FRAME(MaFenetre);

MDZ_DECLARE_FACTORY(RootNode)
MDZ_DECLARE_FACTORY(ClassNode)
MDZ_DECLARE_FACTORY(VarNode)
MDZ_DECLARE_FACTORY(MethodNode)
MDZ_DECLARE_FACTORY(CallbackNode)
MDZ_DECLARE_FACTORY(LayoutNode)

MDZ_DECLARE_FACTORY(FrameNode)
MDZ_DECLARE_FACTORY(DialogNode)
MDZ_DECLARE_FACTORY(ButtonNode)
MDZ_DECLARE_FACTORY(ListBoxNode)
MDZ_DECLARE_FACTORY(TextFieldNode)
MDZ_DECLARE_FACTORY(TextAreaNode)
MDZ_DECLARE_FACTORY(LabelNode)
MDZ_DECLARE_FACTORY(CheckBoxNode)
MDZ_DECLARE_FACTORY(ComboBoxNode)
MDZ_DECLARE_FACTORY(PanelNode)
MDZ_DECLARE_FACTORY(DockBarNode)
MDZ_DECLARE_FACTORY(ToolBarNode)
MDZ_DECLARE_FACTORY(ToolButtonNode)
MDZ_DECLARE_FACTORY(ToolBarSeparatorNode)
MDZ_DECLARE_FACTORY(ToolComboNode)

MDZ_DECLARE_FACTORY(MenuBarNode)
MDZ_DECLARE_FACTORY(MenuNode)
MDZ_DECLARE_FACTORY(MenuItemNode)
MDZ_DECLARE_FACTORY(MenuSeparatorNode)
MDZ_DECLARE_FACTORY(TimerNode)
MDZ_DECLARE_FACTORY(LayoutBorderNode)
MDZ_DECLARE_FACTORY(RadioButtonNode)
MDZ_DECLARE_FACTORY(SliderNode)
MDZ_DECLARE_FACTORY(TabCtrlNode)
MDZ_DECLARE_FACTORY(TreeCtrlNode)
MDZ_DECLARE_FACTORY(TableNode)
MDZ_DECLARE_FACTORY(SocketClientNode)
MDZ_DECLARE_FACTORY(SocketServerNode)
MDZ_DECLARE_FACTORY(TabPanelNode)

Prop::Prop(int type, MdzString name, MdzString label, MdzString value)
{
	this->type = type;
	this->name = name;
	this->value = value;
	this->label = label;
	pCtrl = NULL;
}

void Prop::Create(MdzLayout& layout)
{
	switch(type)
	{
	case SHORTCUT:
		pCtrl = AddShortCut(layout, label, value);
		break;

	case BITMAP:
		pCtrl = AddLabelledBitmap(layout, label, value);
		break;

	case TEXT:
		pCtrl = AddLabelledTextField(layout, label, value);
		break;

	case DIGIT:
		pCtrl = AddLabelledTextField(layout, label, value);
		((MdzTextField*)pCtrl)->SetDigitOnly(TRUE);
		break;


	case CHOICE:
		pCtrl = AddLabelledComboBox(layout, label, value);
		break;

	case BOOL:
		pCtrl = AddLabelledCheckBox(layout, label, value);
		break;

	case COLOR:
		pCtrl = AddLabelledColor(layout, label, value);
		break;

	case FONT:
		pCtrl = AddLabelledFont(layout, label, value);
		break;
	}
}

MdzString Prop::GetValue()
{
	switch(type)
	{
	case TEXT:
	case DIGIT:
	case COLOR:
	case FONT:
	case BITMAP:
		return ((MdzTextField*)pCtrl)->GetText();

	case SHORTCUT:
		return MdzString::Format("%d", ((ShortCutCtrl*)pCtrl)->GetValue());

	case CHOICE:
		return((MdzComboBox*)pCtrl)->GetSelItem();

	case BOOL:
		//return MdzString::Format("%d", ((MdzCheckBox*)pCtrl)->IsChecked());
		return MdzString::Format("%d", ((MdzComboBox*)pCtrl)->GetSelIndex());
	}
	return "";	
}

void Prop::SetValue(MdzString value)
{
	switch(type)
	{
	case TEXT:
	case COLOR:
	case FONT:
	case BITMAP:
	case DIGIT:
		((MdzTextField*)pCtrl)->SetText(value);
		break;

	case SHORTCUT:
		((ShortCutCtrl*)pCtrl)->SetValue(value.ToInt());
		break;

	case CHOICE:
		((MdzComboBox*)pCtrl)->SetSelItem(value);
		break;

	case BOOL:
		//((MdzCheckBox*)pCtrl)->SetChecked(atoi(value)==1);
		((MdzComboBox*)pCtrl)->SetSelIndex(value.ToInt());
		break;
	}
}

MdzCtrl* Prop::AddLabelledBitmap(MdzLayout& layout, MdzString label, MdzString bmpId)
{
	MdzTextField* pTextField = AddLabelledTextField(layout, label, bmpId);
	pTextField->SetMaxChar(6);
	pTextField->SetSize(180, 20);
	MdzButton* pButton = new MdzButton;
	pButton->SetAutoDelete(TRUE);
	pButton->SetCaption("...");		
	layout.Add(*pButton, 20, 20, MdzLayout::K_RIGHT, 0);
	pButton->SetOnClick(MDZ_ARGCBK1(Prop, pTextField, BtnBitmap_OnClick));
	return pTextField;
}

MdzCtrl* Prop::AddLabelledColor(MdzLayout& layout, MdzString label, MdzString color)
{
	MdzTextField* pTextField = AddLabelledTextField(layout, label, color);
	pTextField->SetMaxChar(6);
	pTextField->SetSize(180, 20);
	MdzButton* pButton = new MdzButton;
	pButton->SetAutoDelete(TRUE);
	pButton->SetCaption("...");		
	layout.Add(*pButton, 20, 20, MdzLayout::K_RIGHT, 0);
	pButton->SetOnClick(MDZ_ARGCBK1(Prop, pTextField, BtnColor_OnClick));
	return pTextField;
}

MdzCtrl* Prop::AddLabelledFont(MdzLayout& layout, MdzString label, MdzString font)
{
	MdzTextField* pTextField = AddLabelledTextField(layout, label, font);
	pTextField->SetSize(180, 20);
	MdzButton* pButton = new MdzButton;
	pButton->SetAutoDelete(TRUE);
	pButton->SetCaption("...");		
	layout.Add(*pButton, 20, 20, MdzLayout::K_RIGHT, 0);
	pButton->SetOnClick(MDZ_ARGCBK1(Prop, pTextField, BtnFont_OnClick));
	return pTextField;
}

void Prop::BtnFont_OnClick(LPVOID pArg)
{
	MdzTextField* pTextField = (MdzTextField*)pArg;
	MdzStrVector vector;
	pTextField->GetText().Tokenize(",", vector);
	MdzFont font;
	if (vector.GetCount() == 3)
		font = MdzFont(vector[0], vector[1].ToInt(), vector[2].ToInt());
	if (font.ChooseFont())
		pTextField->SetText(font.ToString());
}
	
void Prop::BtnColor_OnClick(LPVOID pArg)
{
	MdzTextField* pTextField = (MdzTextField*)pArg;
	MdzColor color = pTextField->GetText().ToIntHex();
	if (color.ChooseColor())
		pTextField->SetText(MdzString::Format("%X", color));
}



MdzCtrl* Prop::AddLabelledCheckBox(MdzLayout& layout, MdzString label, MdzString isChecked)
{
	//MdzCheckBox* pCheckBox = new MdzCheckBox;
	MdzComboBox* pCheckBox = new MdzComboBox;
	pCheckBox->AddItem("FALSE");
	pCheckBox->AddItem("TRUE");

	pCheckBox->SetAutoDelete(TRUE);
	AddLabelledCtrl(layout, label, *pCheckBox);
	//pCheckBox->SetChecked(isChecked.ToInt() == 1);
	pCheckBox->SetSelIndex(isChecked.ToInt());
	pCheckBox->SetSize(200, 100);
	return pCheckBox;
}

MdzTextField* Prop::AddLabelledTextField(MdzLayout& layout, MdzString label, MdzString value)
{
	MdzTextField* pTextField = new MdzTextField;
	pTextField->SetAutoDelete(TRUE);
	pTextField->SetText(value);
	AddLabelledCtrl(layout, label, *pTextField);
	return pTextField;
}

void Prop::AddLabelledCtrl(MdzLayout& layout, MdzString label, MdzCtrl& ctrl)
{
	layout.AddLabel(label, 100, 20, MdzLayout::K_NEXT_LINE, PROP_INTERSPACE);
	layout.Add(ctrl, 200, 20, MdzLayout::K_RIGHT, 0);
}

MdzComboBox* Prop::AddLabelledComboBox(MdzLayout& layout, MdzString label, MdzString values)
{
	MdzComboBox* pComboBox = new MdzComboBox;

	AddLabelledCtrl(layout, label, *pComboBox);
	pComboBox->SetSize(200, 100);
	MdzStrVector vector;
	values.Tokenize(";", vector);
	int selIndex = -1;
	for(int i = 0; i < (int)vector.GetCount(); i++)
	{
		char* p = vector[i];
		if (*p == '&')
		{
			selIndex = i;
			p++;
		}
		pComboBox->AddItem(p);
	}
	pComboBox->SetSelIndex(selIndex);
	return pComboBox;
}



Form::Form(MdzContainer* pCtrl) : layout(pCtrl, 10, 45)
{
}

void Form::AddProp(int type, MdzString name, MdzString value)
{
	AddProp(type, name, name, value);
}

void Form::AddProp(int type, MdzString name, MdzString label, MdzString value)
{
	Prop* pProp = new Prop(type, name, label, value);
	props.Add(pProp);
	pProp->Create(layout);
}



MdzString Form::GetValue(MdzString name)
{
	for(int i = 0; i < (int)props.GetCount(); i++)
	{
		if (props[i].GetName().Equals(name))
			return props[i].GetValue();
	}
	return "";

}

void Form::Save(MdzXmlWriter& writer)
{
	for(int i = 0; i < (int)props.GetCount(); i++)		
		writer.WriteAttribute(props[i].GetName(), props[i].GetValue());
}

void Form::Load(MdzXmlElement* pElt)
{
	for(int i = 0; i < (int)props.GetCount(); i++)	
	{
		MdzXmlAttribute* pAttr = pElt->GetAttribute(props[i].GetName());
		if (pAttr)
			props[i].SetValue(pAttr->GetValue());
	}

}


OutputStream::OutputStream()
{
	pFile = NULL;
	iLine = 0;
}

OutputStream::~OutputStream()
{
	Close();
}

void OutputStream::Close()
{
	if (pFile)
		fclose(pFile);
	pFile = NULL;
	iLine = 0;
}

BOOL OutputStream::Open(char* fileName)
{
	Close();
	pFile = fopen(fileName, "w");
	return (pFile != NULL);
}

void OutputStream::Write(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char buffer[1024];
	vsprintf(buffer, fmt, args);
	va_end(args);
	fprintf(pFile, buffer);
}

void OutputStream::WriteLn(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char buffer[1024];
	vsprintf(buffer, fmt, args);
	va_end(args);
	//fprintf(pFile, "%d:%s\n", iLine, buffer);
	fprintf(pFile, "%s\n", buffer);
	iLine++;
}

void OutputStream::WriteLn()
{
	//fprintf(pFile, "%d\n", iLine);
	fprintf(pFile, "\n");
	iLine++;
}



MyDialog::MyDialog()
{ 
	pFocusCtrl = NULL;
	btnOK.SetCaption("OK");
	btnCancel.SetCaption("Cancel");
	btnOK.SetOnClick(MDZ_CBK1(MyDialog, btnOK_OnClick));
	btnCancel.SetOnClick(MDZ_CBK1(MyDialog, btnCancel_OnClick));
	SetDefaultButton(btnOK);
}

void MyDialog::AddOKCancelButtons(MdzLayout& layout, int spacing)
{
	layout.Add(btnCancel, 60, 25, MdzLayout::K_BOTTOM_RIGHT, spacing);
	layout.Add(btnOK, 60, 25, MdzLayout::K_LEFT);
}

void MyDialog::AddLabelledCtrl(MdzLayout& layout, MdzString label, MdzCtrl& ctrl)
{
	if (!pFocusCtrl)
		pFocusCtrl = &ctrl;
	layout.AddLabel(label, 100, 20, MdzLayout::K_NEXT_LINE);
	layout.Add(ctrl, 200, 20, MdzLayout::K_NEXT_LINE, 0);
}

void MyDialog::btnOK_OnClick()
{
	if (Check())
		Close(TRUE);
}

void MyDialog::btnCancel_OnClick()
{
	Close(FALSE);
}

void MyDialog::OnFocus(UINT bGotFocus)
{
	if (bGotFocus && pFocusCtrl)
		pFocusCtrl->RequestFocus();
}

//////////////////////////////////////////////////////////////
#define STRING(a)		MdzString::Format("\"%s\"", (char*)(a))
#define HEX(a)			MdzString::Format("0x%s", (char*)(a))
#define PARAM2(a, b)	MdzString::Format("%s, %s", (char*)(a), (char*)(b))
#define PARAM4(a, b, c, d)	MdzString::Format("%s, %s, %s, %s", (char*)(a), (char*)(b), (char*)(c), (char*)(d))
#define CNT(a, b)		MdzString::Format("%s::K_%s", (char*)(a), (char*)(b))
#define FONT(a)			MdzString::Format("MdzFont(\"%s\"%s)", (char*)a.GetSubString(0, a.GetIndexOf(",")), (char*)a.GetSubString(a.GetIndexOf(",")))


MdzString MyNode::GetParentClassName() 
{
	return ((MyNode*)GetParent())->GetClassName();
}



void MyNode::Save(MdzXmlWriter& writer)
{
	writer.WriteAttribute("Name", GetText());
	writer.WriteAttribute("IsRemovable", MdzString::Format("%d", bIsRemovable));
	form.Save(writer);
}


MyNode::MyNode() : form(&panel)
{
	SetBitmap(IDB_VARIABLE);
	bIsRemovable = TRUE;
	panel.Add(label, MdzBounds(10, 0, 1000, 30));
	panel.SetBackColor(MdzColor::GetSysColor());
	//panel.SetBorder(TRUE);
	label.SetFont(MdzFont("Courier New", 14, 0));
}

void MyNode::Load(MdzXmlElement* pElt)
{
	char* szName = pElt->GetAttributeValue("Name");
	if (szName)
		SetText(szName);
	char* szIsRemovable = pElt->GetAttributeValue("IsRemovable");
	if (szIsRemovable)
		bIsRemovable = atoi(szIsRemovable);
	//label.SetText(GetLabel());
}

void MyNode::LoadProps(MdzXmlElement* pElt)
{
	form.Load(pElt);
}




void MyNode::AddMethodCall(OutputStream& stream, char* methodName, char* methodParameters)
{
	stream.Write("\t");
	if (GetPosition() != 1)
		stream.Write("%s.", (char*)GetRealName());
	stream.WriteLn("%s(%s);", methodName, methodParameters);
}

void MyNode::EncodeHead(OutputStream& stream)
{
	for(int i = 0; i < (int)GetChildCount(); i++)
		((MyNode*)GetChildAt(i))->EncodeHead(stream);
}

void MyNode::EncodeConstructor(OutputStream& stream) 
{
	for(int i = 0; i < (int)GetChildCount(); i++)
		((MyNode*)GetChildAt(i))->EncodeConstructor(stream);
}

void MyNode::EncodeImp(OutputStream& stream) 
{
	for(int i = 0; i < (int)GetChildCount(); i++)
		((MyNode*)GetChildAt(i))->EncodeImp(stream);
}

MdzMenu* MyNode::GetCtxMenu()
{
	/*
	for(int i = 0; i < (int)GetChildCount(); i++)
	{
		MdzMenuItem* pItem = ctxMenu.FindItem(MdzString("Add ") + GetChildAt(i)->GetText());
		if (pItem)
			pItem->SetEnabled(FALSE);
	}
	*/
	return &ctxMenu;
}

void MyNode::InitProps()
{
	label.SetText(GetLabel());
}

void MyNode::Activate(MdzCtrl* pCtrl)
{
	MdzRect rc;
	GetClientRect(pCtrl->GetHandle(), &rc);


	//rc.left += TREEWIDTH;
	//rc.bottom -= TABHEIGHT;

	if (!panel.GetHandle())
	{
		//form.AddBorder();
		panel.SetParent(pCtrl);
		panel.SetBounds(MdzBounds(rc));
		panel.Create();
	}
	else
	{
		panel.SetBounds(MdzBounds(rc));
		panel.SetVisible(TRUE);
	}
	panel.RequestFocus();
}

void MyNode::Unselect()
{
	panel.SetVisible(FALSE);
}

void MyNode::AddNoCheck(MyNode* pNode)
{
	Add(pNode);
	pNode->InitProps();
	SetExpanded(TRUE);
}

BOOL MyNode::AddNode(MyNode* pNode, BOOL bShowWarning)
{
	MdzTreeNode* pCheckNode = IsNameExiting(pNode->GetText());

	if (pCheckNode != NULL)
	{
		if (bShowWarning)
			MdzStdDialog::ShowMessage(MdzString::Format("Name '%s' is already used!",
			(char*)pNode->GetText()), "Warning");
		else
			pCheckNode->Select();
		delete pNode;
		return FALSE;

	}

	AddNoCheck(pNode);
	if (GetPosition() == 1) // ClassNode
		SortChildren();

	return TRUE;
}

MdzTreeNode* MyNode::IsNameExiting(MdzString strName)
{
	for(int i = 0; i < (int)GetChildCount(); i++)
	{
		if (GetChildAt(i)->GetText().Equals(strName))
		{
			return GetChildAt(i);
		}
	}
	return NULL;

}

MdzString MyNode::GetRealName()
{
	MyNode* pParent = (MyNode*)GetParent();

	if (GetPosition() > 2)
		return pParent->GetRealName() + MdzString("_") + GetText();
	else
		return GetText();
}

MdzString MyNode::GetFatherClass()
{
	MdzTreeNode* pNode = this;
	while (pNode->GetPosition() != 1)
		pNode = pNode->GetParent();
	return pNode->GetText();
}


void MyNode::AddMenuItem(MdzString caption, MdzCbk1* pFunc)
{
	ctxMenu.AddItem(caption, pFunc);
}


void MyNode::AddMenuSeparator()
{
	ctxMenu.AddSeparator();
}





LayoutNode* LayoutNode::Create(char* name)
{
	LayoutNode* pNode = new LayoutNode;
	pNode->SetText(name);

	return pNode;
}

void LayoutNode::InitProps()
{
	MyNode::InitProps();
	form.AddProp(Prop::DIGIT, "Left", "10");
	form.AddProp(Prop::DIGIT, "Top", "10");
	AddMenuItem("Add Border", MDZ_CBK1(LayoutNode, addBorder_OnClick));
	AddMenuSeparator();

	AddControl("Button", IDB_BUTTON);
	AddControl("ListBox", IDB_LISTBOX);
	AddControl("ComboBox", IDB_COMBOBOX);
	AddControl("CheckBox", IDB_CHECKBOX);
	AddControl("RadioButton", IDB_RADIOBUTTON);
	AddControl("TextField", IDB_TEXTFIELD);
	AddControl("TextArea", IDB_TEXTAREA);
	AddControl("Label", IDB_LABEL);
	AddControl("Panel");

	AddMenuSeparator();

	AddControl("Slider");
	AddControl("TabCtrl");
	AddControl("TreeCtrl");
	AddControl("Table");

}



void LayoutNode::AddControl(char* ctrlName)
{
	ctxMenu.AddItem(MdzString::Format("Add %s...", ctrlName) , MDZ_ARGCBK1(LayoutNode, ctrlName, addControl_OnClick));
}

void LayoutNode::addControl_OnClick(void* pArg)
{
	MdzString ctrlName = (char*)pArg;

	MdzString name = MdzStdDialog::ShowInput("Name:", MdzString("Add ") + ctrlName);
	if (!name.GetLength())
		return;

	MdzString strNodeName = ctrlName + MdzString("Node");

	MyNode* pNode = (MyNode*)MdzClassFactory::CreateObject(strNodeName);		
	if (!pNode)
	{
		MdzStdDialog::ShowMessage("No factory !!");				
		return;
	}

	pNode->SetText(name);
	AddNode(pNode);
}



//DEL MdzString LayoutNode::GetLabel()
//DEL {
//DEL 	return(MdzString("MdzLayout ") + GetRealName());
//DEL }

void LayoutNode::EncodeConstructor(OutputStream& stream)
{
	MyNode* parent = (MyNode*)GetParent();

	if (parent->GetPosition() == 1)
	{
		stream.WriteLn("\tMdzLayout %s(%s, %s, %s);", 
			(char*)GetRealName(), "this", (char*)form.GetValue("Left"), (char*)form.GetValue("Top")); 
	}
	else
	{
		stream.WriteLn("\tMdzLayout %s(&%s, %s, %s);", 
			(char*)GetRealName(), (char*)parent->GetRealName(), (char*)form.GetValue("Left"), (char*)form.GetValue("Top")); 
	}



	MyNode::EncodeConstructor(stream);
}


/////////////////////////////////////////////////////////////////////

DlgClass::DlgClass()
{
	SetTitle("Add Class");
	MdzLayout layout(this, 10, 10);
	AddLabelledCtrl(layout, "Name", txtClass);
	AddLabelledCtrl(layout, "BaseClass", txtBaseClass);
	AddOKCancelButtons(layout);
	Pack(10, 10);
}


DlgVarChoice::DlgVarChoice()
{
	MdzListBox& basList = list[0];
	MdzListBox& stdList = list[1];
	MdzListBox& comList = list[2];
	MdzListBox& mulList = list[3];
	MdzListBox& netList = list[4];
	MdzListBox& dbList = list[5];

	SetTitle("MDZ_WTK Type");
	Add(tabCtrl, MdzBounds(10, 10, 350, 200));
	tabCtrl.AddTab("Basic", basList);
	tabCtrl.AddTab("Std Ctrls", stdList);
	tabCtrl.AddTab("Extended Ctrls", comList);
	tabCtrl.AddTab("MultiMedia", mulList);
	tabCtrl.AddTab("Network", netList);
	tabCtrl.AddTab("Database", dbList);

	for(int i = 0; i < 6; i++)
	{
		list[i].SetOnDblClick(MDZ_CBK1(DlgVarChoice, list_OnDblClick));
		list[i].SetFont(MdzFont("Courier New", 10));
		list[i].SetSorted(TRUE);
	}

	basList.AddItem("Point");
	basList.AddItem("Size");
	basList.AddItem("Rect");
	basList.AddItem("Bounds");
	basList.AddItem("Font");
	basList.AddItem("Graphic");
	basList.AddItem("Color");
	basList.AddItem("String");
	basList.AddItem("Bitmap");
	basList.AddItem("Vector");

	stdList.AddItem("Button");
	stdList.AddItem("ListBox");
	stdList.AddItem("DirListBox");
	stdList.AddItem("ComboBox");
	stdList.AddItem("DriveComboBox");
	stdList.AddItem("CheckBox");
	stdList.AddItem("TextField");		
	stdList.AddItem("TextArea");
	stdList.AddItem("Label");
	stdList.AddItem("RadioButton");

	comList.AddItem("Panel");
	comList.AddItem("HorzDividerPanel");
	comList.AddItem("VertDividerPanel");
	comList.AddItem("TreeCtrl");
	comList.AddItem("TabCtrl");
	comList.AddItem("Slider");
	comList.AddItem("ToolBar");
	comList.AddItem("Table");
	comList.AddItem("MdiCtrl");

	mulList.AddItem("AviFile");
	mulList.AddItem("VideoStream");
	mulList.AddItem("AudioStream");
	mulList.AddItem("Timer");

	netList.AddItem("SocketClient");
	netList.AddItem("SocketServer");
	netList.AddItem("MailSender");

	dbList.AddItem("OdbcDriver");
	dbList.AddItem("SqlQueryResult");
			
	Pack(10, 10);
}

void DlgVarChoice::list_OnDblClick()
{
	Close(TRUE);
}

MdzString DlgVarChoice::GetSelType()
{
	return MdzString("Mdz") + list[tabCtrl.GetSelIndex()].GetSelItem();
}



MdzString DlgVariable::GetName()
{
	return txtName.GetText();

}

int DlgVariable::GetArraySize()
{
	if (chkArray.IsChecked())
		return txtArraySize.GetText().ToInt();

	return 0;
}

MdzString DlgVariable::GetType() 
{
	MdzString ret = txtType.GetText();
	if (chkPointer.IsChecked())
		ret += "*";

	return ret;
}

DlgVariable::DlgVariable()
{
	SetTitle("Add Variable");
	MdzLayout layout(this, 10, 10);
	AddLabelledCtrl(layout, "Name:", txtName);
	AddLabelledCtrl(layout, "Type:", txtType);
	txtType.SetSize(180, 20);
	layout.Add(button, 20, 20, MdzLayout::K_RIGHT, 0);
	layout.Add(chkPointer, 100, 20, MdzLayout::K_NEXT_LINE);
	layout.Add(chkArray, 100, 20, MdzLayout::K_RIGHT, 0);
	AddLabelledCtrl(layout, "ArraySize:", txtArraySize);
	AddOKCancelButtons(layout, 20);

	txtArraySize.SetEnabled(FALSE);
	txtArraySize.SetDigitOnly(TRUE);

	button.SetCaption("...");
	chkPointer.SetCaption("Pointer");
	chkArray.SetCaption("Array");
	button.SetOnClick(MDZ_CBK1(DlgVariable, button_OnClick));
	chkArray.SetOnClick(MDZ_CBK1(DlgVariable, chkArray_OnClick));
	Pack(10, 10);
}

BOOL DlgVariable::Check()
{
	if (txtName.GetText().GetLength() == 0)
	{
		MdzStdDialog::ShowMessage("Field 'Name' cannot be empty!", "Warning");
		return FALSE;
	}

	if (txtType.GetText().GetLength() == 0)
	{
		MdzStdDialog::ShowMessage("Field 'Type' cannot be empty!", "Warning");
		return FALSE;
	}

	if (chkArray.IsChecked() && txtArraySize.GetText().GetLength() == 0)
	{
		MdzStdDialog::ShowMessage("Field 'ArraySize' cannot be empty!", "Warning");
		return FALSE;
	}

	if (chkArray.IsChecked() && txtArraySize.GetText().ToInt() == 0)
	{
		MdzStdDialog::ShowMessage("Field 'ArraySize' must be greater than 0!", "Warning");
		return FALSE;
	}

	return TRUE;
}

void DlgVariable::chkArray_OnClick()
{
	txtArraySize.SetEnabled(chkArray.IsChecked());
	if (chkArray.IsChecked())
		txtArraySize.RequestFocus();
	else
		txtName.RequestFocus();
}

void DlgVariable::button_OnClick()
{
	DlgVarChoice dlg;
	if (dlg.Execute())
		txtType.SetText(dlg.GetSelType());
}



BOOL DlgResource::GetBitmapId(MdzString& strInitValue)
{
	MdzDialog dlg;
	MdzListBox list2;
	dlg.SetTitle("Bitmap");
	dlg.Add(list2, MdzBounds(0, 0, 120, 165));
	dlg.Pack();
	list2.AddItem("(None)");
	for(int i = 0; i < (int)list.GetItemCount(); i++)
	{
		list2.AddItem(list.GetItemAt(i));
	}
	list2.SetSelItem(strInitValue);

	list2.SetOnDblClick(MDZ_ARGCBK1(DlgResource, &dlg, list2_OnDblClick));
	if (!dlg.Execute())
		return FALSE;

	strInitValue = list2.GetSelItem();

	return TRUE;
}

void DlgResource::list2_OnDblClick(LPVOID pArg)
{
	((MdzDialog*)pArg)->Close(TRUE);
}

void DlgResource::Init()
{
	iLastIndex = 1;
	for(int i = 0; i < (int)list.GetItemCount(); i++)
	{
		MdzString* pString = (MdzString*)list.GetDataAt(i);
		delete pString;
	}
	list.RemoveAllItems();
	picture.Clear();

}

void DlgResource::WriteHeader(OutputStream& stream)
{
	for(int i = 0; i < (int)list.GetItemCount(); i++)
	{
		stream.WriteLn("#define %s\t%d", (char*)list.GetItemAt(i), i+100);
	}

}

void DlgResource::WriteScript(FILE* pFile)
{
	for(int i = 0; i < (int)list.GetItemCount(); i++)
	{
		MdzString* pString = (MdzString*)list.GetDataAt(i);
		fprintf(pFile, "%s\tBITMAP DISCARDABLE\t\"%s\"\n",
			(char*)list.GetItemAt(i), (char*)*pString);
	}

}

void DlgResource::Load(MdzString strProjectPath, MdzXmlElement* pElt)
{
	Init();
	this->strProjectPath = strProjectPath;
	MdzXmlAttribute* pAttr = pElt->GetAttribute("INDEX");
	if (!pAttr)
		return;
	iLastIndex = atoi(pAttr->GetValue());
	for(int i = 0; i < (int)pElt->GetChildCount(); i++)
	{
		MdzXmlElement* pChild = pElt->GetChildAt(i);
		if (pChild->GetName().Equals("BITMAP"))
		{
			pAttr = pChild->GetAttribute("ID");
			if (!pAttr)
				continue;
			MdzString strId = pAttr->GetValue();
			pAttr = pChild->GetAttribute("FILENAME");
			if (!pAttr)
				continue;
			MdzString strFileName = pAttr->GetValue();
			list.AddItem(strId, new MdzString(strFileName));
		}

	}
}

void DlgResource::Save(MdzXmlWriter& writer)
{
	writer.WriteStartElement("RESOURCES");
	writer.WriteAttribute("INDEX", MdzString::Format("%d", iLastIndex));
	for(int i = 0; i < (int)list.GetItemCount(); i++)
	{
		MdzString* pString = (MdzString*)list.GetDataAt(i);
		writer.WriteStartElement("BITMAP");
		writer.WriteAttribute("ID", list.GetItemAt(i));
		writer.WriteAttribute("FILENAME", *pString);
		writer.WriteEndElement();

	}
	writer.WriteEndElement();
}

DlgResource::DlgResource()
{
	iLastIndex = 1;

	SetTitle("Resources");

	btnImport.SetCaption("Import...");
	btnRename.SetCaption("Rename");
	btnRemove.SetCaption("Remove");
	picture.SetBorder(TRUE);

	btnImport.SetOnClick(MDZ_CBK1(DlgResource, btnImport_OnClick));
	btnRemove.SetOnClick(MDZ_CBK1(DlgResource, btnRemove_OnClick));
	btnRename.SetOnClick(MDZ_CBK1(DlgResource, btnRename_OnClick));
	list.SetOnSelChange(MDZ_CBK1(DlgResource, list_OnSelChange));
	EnableButtons(FALSE);

	MdzLayout layout(this, 10, 10);
	layout.Add(list, 120, 165);
	layout.Add(btnImport, 60, 25, MdzLayout::K_RIGHT);
	layout.Add(btnRename, 60, 25, MdzLayout::K_BOTTOM_LEFT);
	layout.Add(btnRemove, 60, 25, MdzLayout::K_BOTTOM_LEFT);
	layout.Add(picture, 60, 60, MdzLayout::K_BOTTOM_LEFT);
	Pack(10, 10);
}

void DlgResource::EnableButtons(BOOL bIsEnabled)
{
	btnRemove.SetEnabled(bIsEnabled);
	btnRename.SetEnabled(bIsEnabled);
}

void DlgResource::btnRename_OnClick()
{
	int index = list.GetSelIndex();
	if (index == LB_ERR)
		return;
	MdzString strNewId = MdzStdDialog::ShowInput("New identifier:", "Rename");
	if (strNewId.GetLength() == 0)
		return;

	MdzString* pString = (MdzString*)list.GetDataAt(index);
	list.RemoveItemAt(index);
	list.InsertItemAt(strNewId, index, pString);
	list.SetSelIndex(index);
}

void DlgResource::btnRemove_OnClick()
{
	int index = list.GetSelIndex();
	if (index == LB_ERR)
		return;
	
	MdzString* pString = (MdzString*)list.GetDataAt(index);
	DeleteFile(strProjectPath + MdzString("\\") + *pString);
	delete pString;
	list.RemoveItemAt(index);
	//MdzStdDialog::ShowMessage(MdzString::Format("%d", list.GetSelIndex()));
	picture.Clear();
	EnableButtons(FALSE);

}

void DlgResource::list_OnSelChange()
{
	EnableButtons(TRUE);
	int index = list.GetSelIndex();
	MdzString* pString = (MdzString*)list.GetDataAt(index);
	picture.SetBitmap(MdzBitmap(
		strProjectPath + MdzString("\\") + *pString));
}

void DlgResource::btnImport_OnClick()
{
	MdzFileDialog dlg;
	dlg.AddFilter("Bitmap", "bmp");
#ifdef DEV
	dlg.SetInitDirectory(MdzSystem::GetExePath() + MdzString("\\..\\bitmap"));
#else
	dlg.SetInitDirectory(MdzSystem::GetExePath() + MdzString("\\bitmap"));
#endif
	if (!dlg.ShowOpen())
		return;

	CopyFile(dlg.GetFullPathName(), 
		strProjectPath + MdzString("\\") + dlg.GetFileName(), FALSE);

	list.AddItem(MdzString::Format("IDB_BITMAP%d", iLastIndex++),
		new MdzString(dlg.GetFileName()));
}



MdzString DlgMethod::GetRetValue() 
{
	MdzString retValue = txtRetValue.GetText();
	return (retValue.GetLength()) ? retValue : "void";
}

int DlgMethod::GetType()
{
	if (chkVirtual.IsChecked())
		return 1;
	if (chkStatic.IsChecked())
		return 2;
	return 0;
}

DlgMethod::DlgMethod()
{
	SetTitle("Add Method");
	MdzLayout layout(this, 10, 10);
	AddLabelledCtrl(layout, "Name:", txtName);
	AddLabelledCtrl(layout, "Prototype:", txtProto);
	AddLabelledCtrl(layout, "Return value:", txtRetValue);
	layout.Add(chkVirtual, 100, 20, MdzLayout::K_NEXT_LINE);
	chkVirtual.SetCaption("Virtual");
	chkVirtual.SetOnClick(MDZ_CBK1(DlgMethod, chkVirtual_OnClick));
	layout.Add(chkStatic, 100, 20, MdzLayout::K_RIGHT, 0);
	chkStatic.SetCaption("Static");
	chkStatic.SetOnClick(MDZ_CBK1(DlgMethod, chkStatic_OnClick));
	AddOKCancelButtons(layout, 20);
	Pack(10, 10);
}

void DlgMethod::chkVirtual_OnClick()
{
	chkStatic.SetEnabled(!chkVirtual.IsChecked());
}

void DlgMethod::chkStatic_OnClick()
{
	chkVirtual.SetEnabled(!chkStatic.IsChecked());
}
	
BOOL DlgMethod::Check()
{
	if (txtName.GetText().GetLength() == 0)
	{
		MdzStdDialog::ShowMessage("Field 'Name' cannot be empty!", "Warning");
		return FALSE;
	}

	MdzStrVector vector1;
	GetProto().Tokenize(",", vector1);
	for (int i = 0; i < (int)vector1.GetCount(); i++)
	{
		MdzStrVector vector2;
		vector1[i].Tokenize(" ", vector2);
		if (vector2.GetCount() != 2)
		{
			MdzStdDialog::ShowMessage("Bad prototype!");
			return FALSE;
		}
	}

	return TRUE;
}



DlgProject::DlgProject()
{
	SetTitle("New project");
	SetFocusCtrl(txtName);

	MdzLayout layout(this, 10, 10);
	/*
	AddLabelledCtrl(layout, "Name:", txtName);
	AddLabelledCtrl(layout, "Drive:", cbDrive);
	cbDrive.SetSize(200, 150);
	AddLabelledCtrl(layout, "Directory:", lstDirectory);
	lstDirectory.SetSize(200, 150);
	*/
	layout.AddLabel("Name:", 200, 20, MdzLayout::K_RIGHT, 20);
	layout.Save();
	layout.Add(txtName, 200, 20, MdzLayout::K_BOTTOM_LEFT, 0);
	layout.AddLabel("Drive:", 200, 20, MdzLayout::K_BOTTOM_LEFT, 10);
	layout.Add(cbDrive, 200, 20, MdzLayout::K_BOTTOM_LEFT, 0);

	layout.Restore();
	layout.AddLabel("Directory:", 200, 20, MdzLayout::K_RIGHT, 20);
	layout.Add(lstDirectory, 200, 150, MdzLayout::K_BOTTOM_LEFT, 0);

	layout.AddLabel("Path:", 200, 20, MdzLayout::K_NEXT_LINE, 0);
	layout.Add(lblPath, 420, 20, MdzLayout::K_BOTTOM_LEFT, 0);

	lstDirectory.SetOnDblClick(MDZ_CBK1(DlgProject, lstDirectory_OnDblClick));
//	lblPath.SetBackColor(MdzColor::GetSysColor());
	lblPath.SetBackColor(MdzColor::K_WHITE);
	lblPath.SetBorder(TRUE);
	lstDirectory_OnDblClick();

	AddOKCancelButtons(layout);
	cbDrive.SetOnSelChange(MDZ_CBK1(DlgProject, cbDrive_OnSelChange));
	Pack(10, 10);

}


BOOL DlgProject::Check()
{
	if (!GetName().GetLength())
	{
		MdzStdDialog::ShowMessage("Please enter a name!", "Warning");
		return FALSE;
	}
	return TRUE;
}

void DlgProject::cbDrive_OnSelChange()
{
	lstDirectory.SetDrive(cbDrive.GetDrive());
}




DlgCode::DlgCode(char* fileName)
{
	SetTitle("Code");
	Add(textArea, MdzBounds(0, 0, 300, 450));
	textArea.SetTabSize(3);
	Pack();

	MdzStream stream;
	if (stream.Open(fileName, MdzStream::K_READ))
	{
		textArea.SetText(stream.ReadText());
	}
}




////////////////////////////////////////////////////////////////////


ClassNode* ClassNode::Create(MdzString name, MdzString baseClass)
{
	ClassNode* pNode = new ClassNode;
	pNode->SetText(name);
	pNode->baseClass = baseClass;
	pNode->addInitDoneMethods();
	return pNode;
}




VarNode* ClassNode::GetVarNode(MdzString strName)
{
	MyNode* pNode = (MyNode*)GetNext();

	while (1)
	{
		if (!pNode)
			break;
		
		if (pNode->GetPosition() == GetPosition())
			break;

		if (pNode->IsInstanceOf("VarNode") && pNode->GetRealName().Equals(strName))
			return (VarNode*)pNode;

		pNode = (MyNode*)pNode->GetNext();
	}
	return NULL;
}

void ClassNode::InitProps()
{
	MyNode::InitProps();
	AddMenuItem("Add Variable...", MDZ_CBK1(ClassNode, addVariable_OnClick));
	AddMenuItem("Add Method...", MDZ_CBK1(ClassNode, addMethod_OnClick));
}



void ClassNode::Save(MdzXmlWriter& writer)
{
	MyNode::Save(writer);
	writer.WriteAttribute("BaseClass", baseClass);
}

void ClassNode::Load(MdzXmlElement* pElt)
{
	MyNode::Load(pElt);
	MdzXmlAttribute* pAttr = pElt->GetAttribute("BaseClass");
	if (pAttr)
		baseClass = pAttr->GetValue();


}

MdzString ClassNode::GetLabel()
{
	MdzString str = MdzString("class ") + GetText();
	if (baseClass.GetLength())
		str += MdzString(": public ") + baseClass;
	else
		str += MdzString(": public MdzObject");

	return(str);
}

void ClassNode::EncodeHead(OutputStream& stream)
{
	stream.WriteLn("%s", (char*)GetLabel());
	stream.WriteLn("{");
	stream.WriteLn("public:");
	stream.WriteLn("\tDEFINE_STD_METHODS(%s, %s)", (char*)GetText(), 
		(baseClass.GetLength() ? (char*)baseClass : "MdzObject"));
	stream.WriteLn("\t%s();", (char*)GetText());
	stream.WriteLn("\t~%s();", (char*)GetText());
	MyNode::EncodeHead(stream);
	stream.WriteLn("};");
	stream.WriteLn();

}

void ClassNode::EncodeImp(OutputStream& stream) 
{
	stream.WriteLn("%s::%s()", (char*)GetText(), (char*)GetText());
	stream.WriteLn("{");
	EncodeConstructor(stream);
	AddMethodCall(stream, "Init", "");
	stream.WriteLn("}");
	stream.WriteLn();

	stream.WriteLn("%s::~%s()", (char*)GetText(), (char*)GetText());
	stream.WriteLn("{");
	AddMethodCall(stream, "Done", "");
	stream.WriteLn("}");
	stream.WriteLn();


	MyNode::EncodeImp(stream);

}





MdzString MethodNode::GetCode()
{
	MdzString ret = "";
	MdzString strCode = code.GetText();
	for(UINT i = 0; i < (UINT)strCode.GetLength(); i++)
		ret += MdzString::Format("%02X", strCode[i]);

	return ret;
}


void MethodNode::Decode(MdzString strCode)
{
	for(UINT i = 0; i < (UINT)strCode.GetLength(); i+=2)
		code.Append(MdzString::Format("%c", strCode.GetSubString(i, 2).ToIntHex()));
}





void MethodNode::Activate(MdzCtrl* pCtrl)
{
	OutputDebugString("MethodNode::Activate\n");
	MyNode::Activate(pCtrl);
	code.SetSelection(0, 0);
}


void MethodNode::SetCode(char* strCode)
{
	code.SetText(strCode);
}

MethodNode* MethodNode::Create(MdzString name, MdzString proto, MdzString retValue, int type)
{
	MethodNode* pNode = new MethodNode;
	pNode->SetText(name);
	pNode->proto = proto;
	pNode->retValue = retValue;
	pNode->type = type;

	return pNode;
}


void MethodNode::InitProps()
{
	MyNode::InitProps();

	code.SetFont(MdzFont("Courier New", 9));
	code.SetTabSize(3);
	panel.Add(code);
	panel.SetOnSize(MDZ_CBK2(MethodNode, panel_OnSize));
	code.SetOnChar(MDZ_CBK6(MethodNode, code_OnChar));
	code.SetOnEnter(MDZ_CBK1(MethodNode, code_OnEnter));
	code.SetOnLButtonDblClick(MDZ_CBK2(MethodNode, code_OnLButtonDblClick));
	GetParameters();
}

void MethodNode::Load(MdzXmlElement* pElt)
{
	MyNode::Load(pElt);
	MdzXmlAttribute* pAttr = pElt->GetAttribute("Proto");
	if (pAttr)
		proto = pAttr->GetValue();
	
	pAttr = pElt->GetAttribute("RetValue");
	if (pAttr)
		retValue = pAttr->GetValue();

	pAttr = pElt->GetAttribute("Type");
	if (pAttr)
		type = atoi(pAttr->GetValue());

	pAttr = pElt->GetAttribute("Code");
	if (pAttr)
		Decode(pAttr->GetValue());
	
}

void MethodNode::Save(MdzXmlWriter& writer)
{
	MyNode::Save(writer);
	writer.WriteAttribute("Proto", proto);
	writer.WriteAttribute("RetValue", retValue);
	writer.WriteAttribute("Type", MdzString::Format("%d", type));
	writer.WriteAttribute("Code", GetCode());


}

MdzString MethodNode::GetLabel()
{
	MdzString str = MdzString::Format("%s %s(%s)",
		(char*)retValue, (char*)GetRealName(), (char*)proto);
	switch (type)
	{
	case 1:
		str = MdzString("virtual ") + str;
		break;
	case 2:
		str  = MdzString("static ") + str;
		break;
	}

	return (str);
}

void MethodNode::panel_OnSize(int width, int height)
{
	code.SetBounds(0, 40, width, height-40);
}

void MethodNode::EncodeHead(OutputStream& stream)
{
	stream.WriteLn("\t%s;", (char*)GetLabel()); 
}

void MethodNode::EncodeImp(OutputStream& stream)
{
	stream.WriteLn("%s %s::%s(%s)", 
		(char*)retValue, (char*)GetFatherClass(), (char*)GetRealName(), (char*)proto);
	stream.WriteLn("{");
	iStartLine = stream.GetLineIndex();
	/*
	for(int i = 0; i < (int)code.GetLineCount(); i++)
		fprintf(pFile, "\t%s\n", (char*)code.GetLineAt(i));
	*/
	MdzString strCode = code.GetText();
	LPSTR pBuff = (LPSTR)strCode;

	while (1)
	{
		LPSTR pSearch = strstr(pBuff, "\r\n");
		if (!pSearch)
			break;
		*pSearch = 0;
		stream.WriteLn("\t%s", pBuff);
		pBuff = pSearch+2;
	}
	stream.WriteLn("\t%s", pBuff);


	iEndLine = stream.GetLineIndex();
	stream.WriteLn("}");
	stream.WriteLn();

}



CallbackNode* CallbackNode::Create(MdzString name, MdzString proto, MdzString retValue)
{
	CallbackNode* pNode = new CallbackNode;
	pNode->SetText(name);
	pNode->proto = proto;
	pNode->retValue = retValue;
	return pNode;
}



void CallbackNode::EncodeConstructor(OutputStream& stream)
{
	MyNode* pParent = (MyNode*)GetParent();

	stream.WriteLn("\t%s.Set%s(MDZ_CBK%d(%s, %s));", 
		(char*)pParent->GetRealName(),
		(char*)GetText(), GetMacroNb(),
		(char*)GetFatherClass(),
		(char*)GetRealName());
}

int CallbackNode::GetMacroNb()
{
	MdzStrVector vector;
	proto.Tokenize(" ,", vector);
	int nbParam = vector.GetCount() / 2;
	if (nbParam == 0 && retValue.Equals("void"))
		return 1;
	if (nbParam == 2 && retValue.Equals("void") && vector[0].Equals("int") &&
		vector[2].Equals("int"))
		return 2;
	if (nbParam == 1 && retValue.Equals("void") &&
		vector[0].Equals("MdzGraphic&"))
		return 4;
	if (nbParam == 1 && retValue.Equals("void") &&
		vector[0].Equals("UINT"))
		return 6;

	return 0;
}



void VarNode::AddCallbackNode(MdzString name, MdzString proto, MdzString retValue)
{
	AddNode(CallbackNode::Create(name, proto, retValue), FALSE);
}


VarNode* VarNode::Create(MdzString name, MdzString type, int size)
{
	VarNode* pNode = new VarNode;
	pNode->size = size;
	pNode->SetText(name);
	pNode->type = type;

	return pNode;
}

void VarNode::Load(MdzXmlElement* pElt)
{
	MyNode::Load(pElt);
	MdzXmlAttribute* pAttr = pElt->GetAttribute("Type");
	if (pAttr)
		type = pAttr->GetValue();
	pAttr = pElt->GetAttribute("ArraySize");

	size = (pAttr) ? atoi(pAttr->GetValue()) : 0;

}

void VarNode::Save(MdzXmlWriter& writer)
{
	MyNode::Save(writer);
	writer.WriteAttribute("Type", type);
	writer.WriteAttribute("ArraySize", MdzString::Format("%d", size));
	
}

MdzString VarNode::GetLabel()
{
	MdzString ret = type + MdzString(" ") + GetRealName();
	if (size > 0)
		ret += MdzString::Format("[%d]", size);
	return ret;
}

void VarNode::EncodeHead(OutputStream& stream)
{
	stream.WriteLn("\t%s;", (char*)GetLabel());
	MyNode::EncodeHead(stream);
}





//DEL TimerNode* TimerNode::Create(char* name)
//DEL {
//DEL 	TimerNode* pNode = new TimerNode;
//DEL 	pNode->SetText(name);
//DEL 	return pNode;
//DEL }

void TimerNode::EncodeConstructor(OutputStream& stream)
{
	VarNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();

	AddMethodCall(stream, "SetDelay", form.GetValue("Delay"));

}

void TimerNode::InitProps()
{
	VarNode::InitProps();
	AddMenuItem("Add OnTimeout", MDZ_CBK1(TimerNode, addOnTimeout_OnClick));
	form.AddProp(Prop::DIGIT, "Delay", "Delay (ms)", "1000");

}

void TimerNode::addOnTimeout_OnClick()
{
	AddCallbackNode("OnTimeout", "", "void");
}



BOOL MenuItemNode::CanMove() 
{
	return (GetParentClassName().Equals("MenuNode"));

}

MenuItemNode* MenuItemNode::Create(char* name)
{
	MenuItemNode* pNode = new MenuItemNode;
	pNode->SetText(name);
	pNode->type = "MdzMenuItem";
	return pNode;
}

void MenuItemNode::EncodeConstructor(OutputStream& stream)
{
	VarNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	//MdzString strParent = MdzString(pParent->GetClassName());
	if (pParent->GetClassName().Equals("MenuNode"))
	{
		stream.WriteLn("\t%s.Add(%s, %s);", (char*)pParent->GetRealName(),
			(char*)GetRealName(), (char*)STRING(form.GetValue("Caption")));
	}

	AddMethodCall(stream, "SetEnabled", form.GetValue("Enabled"));
	AddMethodCall(stream, "SetChecked", form.GetValue("Checked"));
	if (!form.GetValue("Bitmap").Equals("(None)"))
		AddMethodCall(stream, "SetBitmap", form.GetValue("Bitmap"));
	DWORD iShortcut = form.GetValue("Shortcut").ToInt();
	if (LOWORD(iShortcut) != 0)
		AddMethodCall(stream, "SetShortcut", MdzString::Format("%d, %d", HIWORD(iShortcut), LOWORD(iShortcut)));

}

void MenuItemNode::InitProps()
{
	VarNode::InitProps();
	AddMenuItem("Add OnClick", MDZ_CBK1(MenuItemNode, addOnClick_OnClick));
	form.AddProp(Prop::TEXT, "Caption", "MenuItem");
	form.AddProp(Prop::BOOL, "Enabled", "1");
	form.AddProp(Prop::BOOL, "Checked", "0");
	form.AddProp(Prop::BITMAP, "Bitmap", "(None)");
	form.AddProp(Prop::SHORTCUT, "Shortcut", "");

}

void MenuItemNode::addOnClick_OnClick()
{
	AddCallbackNode("OnClick", "", "void");
}


MenuSeparatorNode::MenuSeparatorNode()
{
	SetText("Separator");
}

void MenuSeparatorNode::EncodeConstructor(OutputStream& stream)
{
	MyNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	//MdzString strParent = MdzString(pParent->GetClassName());
	if (pParent->GetClassName().Equals("MenuNode"))
	{
		stream.WriteLn("\t%s.AddSeparator();", (char*)pParent->GetRealName());
			
	}
}



BOOL MenuNode::CanMove() 
{		
	return (GetParentClassName().Equals("MenuNode") || GetParentClassName().Equals("MenuBarNode"));

}

MenuNode* MenuNode::Create(char* name)
{
	MenuNode* pNode = new MenuNode;
	pNode->SetText(name);
	return pNode;
}

void MenuNode::EncodeConstructor(OutputStream& stream)
{
	VarNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	//MdzString strParent = MdzString(pParent->GetClassName());
	if (pParent->GetClassName().Equals("MenuBarNode") || pParent->GetClassName().Equals("MenuNode"))
	{
		stream.WriteLn("\t%s.Add(%s, %s);", (char*)pParent->GetRealName(),
			(char*)GetRealName(), (char*)STRING(form.GetValue("Caption")));
	}
}


void MenuNode::InitProps()
{
	VarNode::InitProps();
	if (GetParentClassName().Equals("MenuBarNode") || GetParentClassName().Equals("MenuNode"))
		form.AddProp(Prop::TEXT, "Caption", "Menu");

	AddMenuItem("Add Menu...", MDZ_CBK1(MenuNode, addMenu_OnClick));
	AddMenuItem("Add MenuItem...", MDZ_CBK1(MenuNode, addMenuItem_OnClick));
	AddMenuItem("Add Separator", MDZ_CBK1(MenuNode, addSeparator_OnClick));
	AddMenuSeparator();
	AddMenuItem("Add OnClick", MDZ_CBK1(MenuNode, addOnClick_OnClick));

}

void MenuNode::addOnClick_OnClick()
{
	AddCallbackNode("OnClick", "", "void");
}

void MenuNode::addSeparator_OnClick()
{
	AddNoCheck(new MenuSeparatorNode);
}

void MenuNode::addMenu_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add Menu");
	if (strName.GetLength())
		AddNode(MenuNode::Create(strName));
}

void MenuNode::addMenuItem_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add MenuItem");
	if (strName.GetLength())
		AddNode(MenuItemNode::Create(strName));
}




MenuBarNode::MenuBarNode()
{
	type = "MdzMenuBar";
	SetText("menuBar");
}

void MenuBarNode::EncodeConstructor(OutputStream& stream)
{
	VarNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	if (pParent->GetClassName().Equals("FrameNode"))
	{
		stream.WriteLn("\tSetMenuBar(%s);", (char*)GetRealName());
	}
}

void MenuBarNode::InitProps()
{
	VarNode::InitProps();
	AddMenuItem("Add Menu...", MDZ_CBK1(MenuBarNode, addMenu_OnClick));

}

void MenuBarNode::addMenu_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add Menu");
	if (strName.GetLength())
		AddNode(MenuNode::Create(strName));
}





BOOL CtrlNode::CanMove() 
{
	return (GetParentClassName().Equals("LayoutNode"));

}



void CtrlNode::EncodeConstructor(OutputStream& stream)
{
	VarNode::EncodeConstructor(stream);

	MyNode* pParent = (MyNode*)GetParent();
	if (GetParentClassName().Equals("LayoutNode"))
	{
		if (form.GetValue("LayoutAction").GetIndexOf("RESTORE") >= 0)
			stream.WriteLn("\t%s.Restore();", (char*)pParent->GetRealName());

		if (GetLabelText().GetLength() > 0)
		{
			stream.WriteLn("\t%s.AddLabel(%s, %s, %s, %s, %s);", (char*)pParent->GetRealName(),
				(char*)STRING(GetLabelText()),
				(char*)GetLabelWidth(),
				(char*)GetHeight(),
				(char*)CNT("MdzLayout", GetLayoutMode()),
				(char*)GetLayoutSpacing());

			stream.WriteLn("\t%s.Add(%s, %s, %s, MdzLayout::K_RIGHT, 0);", (char*)pParent->GetRealName(),
				(char*)GetRealName(), 
				(char*)GetWidth(),
				(char*)GetHeight());
		}
		else
		{

			stream.WriteLn("\t%s.Add(%s, %s, %s, %s, %s);", (char*)pParent->GetRealName(),
				(char*)GetRealName(), 
				(char*)GetWidth(),
				(char*)GetHeight(),
				(char*)CNT("MdzLayout", GetLayoutMode()),
				(char*)GetLayoutSpacing());
		}

		if (form.GetValue("LayoutAction").GetIndexOf("SAVE") >= 0)
			stream.WriteLn("\t%s.Save();", (char*)pParent->GetRealName());

	}
	else
	{
		AddMethodCall(stream, "SetBounds", PARAM4(form.GetValue("Left"),
			form.GetValue("Top"), GetWidth(), GetHeight()));
	}

	AddMethodCall(stream, "SetEnabled", form.GetValue("Enable"));


}

void CtrlNode::InitProps()
{
	/*
	form.SetContainer(&propertiesPane, 0);
	propertiesPane.SetBackColor(MdzColor::GetSysColor());
	//propertiesPane.SetVertScrollbar(1000, 100);
	panel.Add(propertiesPane);
	panel.SetOnSize(MDZ_CBK2(CtrlNode, panel_OnSize));
	*/
	MyNode::InitProps();
	MyNode* pParent = (MyNode*)GetParent();
	if (pParent->GetClassName().Equals("LayoutNode"))
	{
		form.AddProp(Prop::CHOICE, "LayoutMode", "&RIGHT;LEFT;BOTTOM_LEFT;BOTTOM_RIGHT;NEXT_LINE");  
		form.AddProp(Prop::DIGIT, "LayoutSpacing", "10");
		form.AddProp(Prop::CHOICE, "LayoutAction", "&NONE;SAVE;RESTORE;RESTORE & SAVE");
	}
	else
	{
		form.AddProp(Prop::DIGIT, "Left", "10");
		form.AddProp(Prop::DIGIT, "Top", "10");
	}
	form.AddProp(Prop::DIGIT, "Width", "100");
	form.AddProp(Prop::DIGIT, "Height", "100");
	form.AddProp(Prop::BOOL, "Enable", "1");
}
	


ButtonNode::ButtonNode()
{
	type = "MdzButton";
	SetBitmap(IDB_BUTTON);
}


void ButtonNode::InitProps()
{
	CtrlNode::InitProps();

	form.SetValue("Height", "25");

	AddMenuItem("Add OnClick", MDZ_CBK1(ButtonNode, addOnClick_OnClick));
	form.AddProp(Prop::TEXT, "Caption", "button");
	form.AddProp(Prop::FONT, "Font", "Microsoft Sans Serif,8,0");
	form.AddProp(Prop::BITMAP, "Bitmap", "(None)");
}

void ButtonNode::addOnClick_OnClick()
{
	AddCallbackNode("OnClick", "", "void");
}

void ButtonNode::EncodeConstructor(OutputStream& stream)
{
	CtrlNode::EncodeConstructor(stream);

	MdzString strBitmap = form.GetValue("Bitmap") ;

	if (strBitmap.Equals("(None)"))
		AddMethodCall(stream, "SetCaption", STRING(form.GetValue("Caption")));
	else
		AddMethodCall(stream, "SetBitmap", strBitmap);

	AddMethodCall(stream, "SetFont", FONT(form.GetValue("Font")));

}



ListBoxNode::ListBoxNode()
{
	type = "MdzListBox";
	SetBitmap(IDB_LISTBOX);

}


void ListBoxNode::InitProps()
{
	CtrlNode::InitProps();
	form.AddProp(Prop::FONT, "Font", "Microsoft Sans Serif,8,0");
	form.AddProp(Prop::BOOL, "Sorted", "0");
	form.AddProp(Prop::BOOL, "MultiSelection", "0");

	AddMenuItem("Add OnSelChange", MDZ_CBK1(ListBoxNode, addOnSelChange_OnClick));
	AddMenuItem("Add OnDblClick", MDZ_CBK1(ListBoxNode, addOnDblClick_OnClick));

}

void ListBoxNode::addOnSelChange_OnClick()
{
	AddCallbackNode("OnSelChange", "", "void");
}

void ListBoxNode::addOnDblClick_OnClick()
{
	AddCallbackNode("OnDblClick", "", "void");
}

void ListBoxNode::EncodeConstructor(OutputStream& stream)
{
	CtrlNode::EncodeConstructor(stream);
	AddMethodCall(stream, "SetFont", FONT(form.GetValue("Font")));
	AddMethodCall(stream, "SetSorted", form.GetValue("Sorted"));
	AddMethodCall(stream, "SetMultiSelection", form.GetValue("MultiSelection"));
}




ComboBoxNode::ComboBoxNode()
{
	type = "MdzComboBox";
	SetBitmap(IDB_COMBOBOX);

}


void ComboBoxNode::InitProps()
{
	LabelledCtrlNode::InitProps();
	form.AddProp(Prop::FONT, "Font", "Microsoft Sans Serif,8,0");

	AddMenuItem("Add OnSelChange", MDZ_CBK1(ComboBoxNode, addOnSelChange_OnClick));

}

void ComboBoxNode::addOnSelChange_OnClick()
{
	AddCallbackNode("OnSelChange", "", "void");
}


void ComboBoxNode::EncodeConstructor(OutputStream& stream)
{
	LabelledCtrlNode::EncodeConstructor(stream);
	AddMethodCall(stream, "SetFont", FONT(form.GetValue("Font")));
}




	
TextFieldNode::TextFieldNode()
{
	type = "MdzTextField";
	SetBitmap(IDB_TEXTFIELD);
}


void TextFieldNode::InitProps()
{
	LabelledCtrlNode::InitProps();
	form.AddProp(Prop::FONT, "Font", "Microsoft Sans Serif,8,0");
	form.AddProp(Prop::TEXT, "Text", "");
	form.AddProp(Prop::DIGIT, "MaxChar", "10");
	form.AddProp(Prop::BOOL, "Password", "0");
	form.AddProp(Prop::BOOL, "DigitOnly", "0");

}


void TextFieldNode::EncodeConstructor(OutputStream& stream)
{
	LabelledCtrlNode::EncodeConstructor(stream);

	AddMethodCall(stream, "SetFont", FONT(form.GetValue("Font")));
	AddMethodCall(stream, "SetText", STRING(form.GetValue("Text")));
	AddMethodCall(stream, "SetMaxChar", form.GetValue("MaxChar"));
	AddMethodCall(stream, "SetPassword", form.GetValue("Password"));
	AddMethodCall(stream, "SetDigitOnly", form.GetValue("DigitOnly"));
}




	
TextAreaNode::TextAreaNode()
{
	type = "MdzTextArea";
	SetBitmap(IDB_TEXTAREA);

}


void TextAreaNode::InitProps()
{
	CtrlNode::InitProps();
	form.AddProp(Prop::FONT, "Font", "Microsoft Sans Serif,8,0");
	form.AddProp(Prop::DIGIT, "TabSize", "4");
	form.AddProp(Prop::BOOL, "Editable", "1");

}


void TextAreaNode::EncodeConstructor(OutputStream& stream)
{
	CtrlNode::EncodeConstructor(stream);
	AddMethodCall(stream, "SetFont", FONT(form.GetValue("Font")));
	AddMethodCall(stream, "SetTabSize", form.GetValue("TabSize"));
	AddMethodCall(stream, "SetEditable", form.GetValue("Editable"));
}




PanelNode::PanelNode()
{
	type = "MdzPanel";

}


void PanelNode::InitProps()
{
	CtrlNode::InitProps();
	form.AddProp(Prop::BOOL, "Border", "0");
	form.AddProp(Prop::COLOR, "BackColor", "FFFFFF");

	AddMenuItem("Add Layout...", MDZ_CBK1(PanelNode, addLayout_OnClick));
	AddMenuSeparator();
	AddMenuItem("Add OnPaint", MDZ_CBK1(PanelNode, addOnPaint_OnClick));
	AddMenuItem("Add OnSize", MDZ_CBK1(PanelNode, addOnSize_OnClick));
	AddMenuItem("Add OnMouseMove", MDZ_CBK1(PanelNode, addOnMouseMove_OnClick));
	AddMenuItem("Add OnMouseDrag", MDZ_CBK1(PanelNode, addOnMouseDrag_OnClick));
	AddMenuItem("Add OnLeftButtonDown", MDZ_CBK1(PanelNode, addOnLeftButtonDown_OnClick));
	AddMenuItem("Add OnLeftButtonUp", MDZ_CBK1(PanelNode, addOnLeftButtonUp_OnClick));
	AddMenuItem("Add OnRightButtonDown", MDZ_CBK1(PanelNode, addOnRightButtonDown_OnClick));
	AddMenuItem("Add OnRightButtonUp", MDZ_CBK1(PanelNode, addOnRightButtonUp_OnClick));
	AddMenuItem("Add OnKeyDown", MDZ_CBK1(PanelNode, addOnKeyDown_OnClick));
	AddMenuItem("Add OnKeyUp", MDZ_CBK1(PanelNode, addOnKeyUp_OnClick));

}

void PanelNode::addLayout_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add Layout");
	if (strName.GetLength())
	{
		AddNode(LayoutNode::Create(strName));
		SortChildren();
	}
};

void PanelNode::addOnSize_OnClick()
{
	AddCallbackNode("OnSize", "int width, int height", "void");
	SortChildren();
}

void PanelNode::addOnKeyDown_OnClick()
{
	AddCallbackNode("OnKeyDown", "UINT key", "void");
}

void PanelNode::addOnKeyUp_OnClick()
{
	AddCallbackNode("OnKeyUp", "UINT key", "void");
	SortChildren();
}

void PanelNode::addOnPaint_OnClick()
{
	AddCallbackNode("OnPaint", "MdzGraphic& graphic", "void");
	SortChildren();
}

void PanelNode::addOnMouseMove_OnClick()
{
	AddCallbackNode("OnMouseMove", "int x, int y", "void");
	SortChildren();
}

void PanelNode::addOnMouseDrag_OnClick()
{
	AddCallbackNode("OnMouseDrag", "int x, int y", "void");
	SortChildren();
}

void PanelNode::addOnLeftButtonDown_OnClick()
{
	AddCallbackNode("OnLeftButtonDown", "int x, int y", "void");
	SortChildren();
}

void PanelNode::addOnLeftButtonUp_OnClick()
{
	AddCallbackNode("OnLeftButtonUp", "int x, int y", "void");
	SortChildren();
}

void PanelNode::addOnRightButtonDown_OnClick()
{
	AddCallbackNode("OnRightButtonDown", "int x, int y", "void");
	SortChildren();
}

void PanelNode::addOnRightButtonUp_OnClick()
{
	AddCallbackNode("OnRightButtonUp", "int x, int y", "void");
	SortChildren();
}

void PanelNode::EncodeConstructor(OutputStream& stream)
{
	CtrlNode::EncodeConstructor(stream);
	AddMethodCall(stream, "SetBorder", form.GetValue("Border"));
	AddMethodCall(stream, "SetBackColor", HEX(form.GetValue("BackColor")));
	SortChildren();
}




LabelNode::LabelNode()
{
	type = "MdzLabel";
	SetBitmap(IDB_LABEL);

}


void LabelNode::InitProps()
{
	CtrlNode::InitProps();
	form.AddProp(Prop::FONT, "Font", "Microsoft Sans Serif,8,0");
	form.AddProp(Prop::TEXT, "Text", "");
	form.AddProp(Prop::CHOICE, "Alignment", "&LEFT;RIGHT;CENTER");
	form.AddProp(Prop::COLOR, "TextColor", "0");
	form.AddProp(Prop::BOOL, "Opaque", "0");
	form.AddProp(Prop::COLOR, "BackColor", "FFFFFF");
	form.AddProp(Prop::BOOL, "Border", "0");

}


void LabelNode::EncodeConstructor(OutputStream& stream)
{
	CtrlNode::EncodeConstructor(stream);
	AddMethodCall(stream, "SetFont", FONT(form.GetValue("Font")));
	AddMethodCall(stream, "SetText", STRING(form.GetValue("Text")));
	AddMethodCall(stream, "SetAlignment", CNT("MdzLabel", form.GetValue("Alignment")));
	AddMethodCall(stream, "SetTextColor", HEX(form.GetValue("TextColor")));
	AddMethodCall(stream, "SetBorder", form.GetValue("Border"));
	if (form.GetValue("Opaque").Equals("1"))
		AddMethodCall(stream, "SetBackColor", HEX(form.GetValue("BackColor")));
}



ToolComboNode* ToolComboNode::Create(char* name)
{
	ToolComboNode* pNode = new ToolComboNode;
	pNode->SetText(name);
	return pNode;
}


ToolComboNode::ToolComboNode()
{
	type = "MdzComboBox";
}

BOOL ToolComboNode::CanMove() 
{		
	return TRUE;

}
	
void ToolComboNode::EncodeConstructor(OutputStream& stream)
{
	VarNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	stream.WriteLn("\t%s.AddCtrl(%s, %d, 100);", (char*)pParent->GetRealName(),
		(char*)GetRealName(), 
		form.GetValue("Width").ToInt());

}


void ToolComboNode::addOnSelChange_OnClick()
{
	AddCallbackNode("OnSelChange", "", "void");
}


void ToolComboNode::InitProps()
{
	VarNode::InitProps();
	form.AddProp(Prop::DIGIT, "Width", "100");
	AddMenuItem("Add OnSelChange", MDZ_CBK1(ToolComboNode, addOnSelChange_OnClick));
}




ToolButtonNode* ToolButtonNode::Create(char* name)
{
	ToolButtonNode* pNode = new ToolButtonNode;
	pNode->SetText(name);
	return pNode;
}


ToolButtonNode::ToolButtonNode()
{
	type = "MdzToolButton";
}

BOOL ToolButtonNode::CanMove() 
{		
	return (GetParentClassName().Equals("ToolBarNode"));

}
	
void ToolButtonNode::EncodeConstructor(OutputStream& stream)
{
	VarNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	AddMethodCall(stream, "SetToolTips", STRING(form.GetValue("ToolTips")));

	if (pParent->GetClassName().Equals("ToolBarNode"))
	{
		if (!form.GetValue("Bitmap").Equals("(None)"))
		{
			stream.WriteLn("\t%s.AddButton(%s, MdzBitmap(%s), %s);", (char*)pParent->GetRealName(),
				(char*)GetRealName(),
				(char*)form.GetValue("Bitmap"),
				(char*)CNT("MdzToolButton", form.GetValue("Style")));
		}
		else
		{
			stream.WriteLn("\t%s.AddButton(%s, MdzBitmap(), %s);", (char*)pParent->GetRealName(),
				(char*)GetRealName(),
				(char*)CNT("MdzToolButton", form.GetValue("Style")));
		}
	}
}

void ToolButtonNode::addOnClick_OnClick()
{
	AddCallbackNode("OnClick", "", "void");
}


void ToolButtonNode::InitProps()
{
	VarNode::InitProps();
	/*
	form.AddProp(Prop::CHOICE, "StdBitmap", "Standard Bitmap", 
		"&FILEOPEN;FILESAVE;FILENEW;CUT;PASTE;COPY;UNDO;FIND;REPLACE;PRINT;HELP;DELETE");
	*/
	form.AddProp(Prop::BITMAP, "Bitmap", "(None)");
	form.AddProp(Prop::CHOICE, "Style", "&BUTTON;CHECK;CHECKGROUP");
	form.AddProp(Prop::TEXT, "ToolTips", "");
	AddMenuItem("Add OnClick", MDZ_CBK1(ToolButtonNode, addOnClick_OnClick));
}




ToolBarSeparatorNode::ToolBarSeparatorNode()
{
	SetText("Separator");
}

void ToolBarSeparatorNode::EncodeConstructor(OutputStream& stream)
{
	MyNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	if (pParent->GetClassName().Equals("ToolBarNode"))
	{
		stream.WriteLn("\t%s.AddSeparator();", (char*)pParent->GetRealName());
			
	}
}




ToolBarNode* ToolBarNode::Create(char* name)
{
	ToolBarNode* pNode = new ToolBarNode;
	pNode->SetText(name);
	return pNode;
}


ToolBarNode::ToolBarNode()
{
	type = "MdzToolBar";
}

void ToolBarNode::EncodeConstructor(OutputStream& stream)
{
	VarNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	if (pParent->GetClassName().Equals("DockBarNode"))
	{
		stream.WriteLn("\t%s.AddToolBar(%s, %d);", (char*)pParent->GetRealName(),
			(char*)GetRealName(), form.GetValue("Break").ToInt());
	}
}

void ToolBarNode::addButton_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add Button");
	if (strName.GetLength())
		AddNode(ToolButtonNode::Create(strName));
}

void ToolBarNode::addComboBox_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add ComboBox");
	if (strName.GetLength())
		AddNode(ToolComboNode::Create(strName));
}

void ToolBarNode::addSeparator_OnClick()
{
	AddNoCheck(new ToolBarSeparatorNode);
}

void ToolBarNode::InitProps()
{
	VarNode::InitProps();
	form.AddProp(Prop::BOOL, "Break", "1");

	AddMenuItem("Add Button", MDZ_CBK1(ToolBarNode, addButton_OnClick));
	AddMenuItem("Add Separator", MDZ_CBK1(ToolBarNode, addSeparator_OnClick));
	AddMenuItem("Add ComboBox", MDZ_CBK1(ToolBarNode, addComboBox_OnClick));
}




DockBarNode::DockBarNode()
{
	type = "MdzDockBar";
	SetText("dockBar");

}

void DockBarNode::EncodeConstructor(OutputStream& stream)
{
	VarNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	if (pParent->GetClassName().Equals("FrameNode"))
	{
		stream.WriteLn("\tSetDockBar(%s);", (char*)GetRealName());
	}

}

void DockBarNode::addToolBar_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add ToolBar");
	if (strName.GetLength())
		AddNode(ToolBarNode::Create(strName));
}

void DockBarNode::InitProps()
{
	VarNode::InitProps();
	AddMenuItem("Add ToolBar", MDZ_CBK1(DockBarNode, addToolBar_OnClick));
}




CheckBoxNode::CheckBoxNode()
{
	type = "MdzCheckBox";
	SetBitmap(IDB_CHECKBOX);

}

void CheckBoxNode::addOnClick_OnClick()
{
	AddCallbackNode("OnClick", "", "void");
}

void CheckBoxNode::InitProps()
{
	CtrlNode::InitProps();
	form.SetValue("Height", "20");
	form.AddProp(Prop::FONT, "Font", "Microsoft Sans Serif,8,0");
	form.AddProp(Prop::TEXT, "Caption", "");
	form.AddProp(Prop::BOOL, "Checked", "0");
	AddMenuItem("Add OnClick", MDZ_CBK1(CheckBoxNode, addOnClick_OnClick));


}


void CheckBoxNode::EncodeConstructor(OutputStream& stream)
{
	CtrlNode::EncodeConstructor(stream);
	AddMethodCall(stream, "SetFont", FONT(form.GetValue("Font")));
	AddMethodCall(stream, "SetCaption", STRING(form.GetValue("Caption")));
	AddMethodCall(stream, "SetChecked", form.GetValue("Checked"));
}





FrameNode::FrameNode()
{
	SetText("MainFrame");
	baseClass = "MdzFrame";
	SetRemovable(FALSE);
}

FrameNode* FrameNode::Create()
{
	FrameNode* pNode = new FrameNode;
	pNode->addInitDoneMethods();
	return pNode;
}	


void FrameNode::InitProps()
{
	ClassNode::InitProps();
	form.AddProp(Prop::TEXT, "Title", "My Application");
	form.AddProp(Prop::BOOL, "Resizable", "1");
	form.AddProp(Prop::DIGIT, "Width", "400");
	form.AddProp(Prop::DIGIT, "Height", "400");
	form.AddProp(Prop::COLOR, "BackColor", "FFFFFF");

	AddMenuSeparator();
	AddMenuItem("Add OnSize", MDZ_CBK1(FrameNode, addOnSize_OnClick));
	AddMenuItem("Add OnClose", MDZ_CBK1(FrameNode, addOnClose_OnClick));
	AddMenuSeparator();
	AddMenuItem("Add Layout...", MDZ_CBK1(FrameNode, addLayout_OnClick));
	AddMenuItem("Add MenuBar", MDZ_CBK1(FrameNode, addMenuBar_OnClick));
	AddMenuItem("Add DockBar", MDZ_CBK1(FrameNode, addDockBar_OnClick));

}

void FrameNode::addDockBar_OnClick()
{
	AddNode(new DockBarNode, FALSE);
}

void FrameNode::addMenuBar_OnClick()
{
	AddNode(new MenuBarNode, FALSE);
}

void FrameNode::addLayout_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add Layout");
	if (strName.GetLength())
		AddNode(LayoutNode::Create(strName));
}

void FrameNode::addOnSize_OnClick()
{
	AddNode(MethodNode::Create("OnSize", "MdzRect& rc", "void"), FALSE);
}

void FrameNode::addOnClose_OnClick()
{
	MethodNode* pNode = MethodNode::Create("OnClose", "", "BOOL");
	pNode->SetCode("return TRUE;");
	AddNode(pNode, FALSE);
}

void FrameNode::EncodeConstructor(OutputStream& stream)
{
	AddMethodCall(stream, "SetTitle", STRING(form.GetValue("Title")));
	AddMethodCall(stream, "SetResizable", form.GetValue("Resizable"));
	AddMethodCall(stream, "SetSize", PARAM2(form.GetValue("Width"), form.GetValue("Height")));
	AddMethodCall(stream, "SetBackColor", HEX(form.GetValue("BackColor")));

	ClassNode::EncodeConstructor(stream);
}

void FrameNode::EncodeImp(OutputStream& stream)
{
	stream.WriteLn("MDZ_DECLARE_FRAME(%s)", (char*)GetText());
	stream.WriteLn();
	ClassNode::EncodeImp(stream);
}



DialogNode::DialogNode()
{
	baseClass = "MdzDialog";
}

DialogNode* DialogNode::Create(LPSTR strName)
{
	DialogNode* pNode = new DialogNode;
	pNode->SetText(strName);
	pNode->addInitDoneMethods();
	return pNode;
}	


void DialogNode::InitProps()
{
	ClassNode::InitProps();
	form.AddProp(Prop::TEXT, "Title", "My Application");
	form.AddProp(Prop::DIGIT, "Width", "400");
	form.AddProp(Prop::DIGIT, "Height", "400");

	AddMenuSeparator();
	AddMenuItem("Add Layout...", MDZ_CBK1(DialogNode, addLayout_OnClick));
	AddMenuSeparator();
	AddMenuItem("Add OnFocus", MDZ_CBK1(DialogNode, addOnFocus_OnClick));

}

void DialogNode::addOnFocus_OnClick()
{
	MethodNode* pNode = MethodNode::Create("OnFocus", "UINT bGotFocus", "void");
	AddNode(pNode, FALSE);
};

void DialogNode::addLayout_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add Layout");
	if (strName.GetLength())
		AddNode(LayoutNode::Create(strName));
};


void DialogNode::EncodeConstructor(OutputStream& stream)
{
	AddMethodCall(stream, "SetTitle", STRING(form.GetValue("Title")));
	AddMethodCall(stream, "SetSize", PARAM2(form.GetValue("Width"), form.GetValue("Height")));

	ClassNode::EncodeConstructor(stream);
}





RootNode* RootNode::Create()
{
	RootNode* pNode = new RootNode;
	pNode->AddNode(FrameNode::Create());
	pNode->SetRemovable(FALSE);
	return pNode;
}


RootNode::RootNode()
{
	SetText("Classes");
	SetBitmap(IDB_ROOT);
	AddMenuItem("Add Class...", MDZ_CBK1(RootNode, addClass_OnClick));
	AddMenuSeparator();
	AddMenuItem("Add DialogBox...", MDZ_CBK1(RootNode, addDialog_OnClick));
}




void RootNode::addDialog_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add DialogBox");
	if (strName.GetLength())
		AddNode(DialogNode::Create(strName));
}

void RootNode::addClass_OnClick()
{
	DlgClass dlg;
	if (dlg.Execute())
		AddNode(ClassNode::Create(dlg.GetName(), dlg.GetBaseClass()));
}




////////////////////////////////////////////////////////////////////////////





void MaFenetre::listbox_OnDblClick()
{
int iLine = (int)buildList.GetDataAt(buildList.GetSelIndex());
if (iLine != 0)
{
	MethodNode* pNode = FindMethodNode(iLine);
	if (pNode)
	{
		pNode->Select();
		MdzTextArea* pTextArea = pNode->GetTextArea();

		
		while (iLine > pNode->GetStartLine() && 
			pTextArea->GetLineAt(iLine - pNode->GetStartLine()).GetLength() == 0)
			iLine--;
		
		
		int iOffset = iLine - pNode->GetStartLine();
		pTextArea->SelectLine(iOffset);
	}

}
}

BOOL MaFenetre::CreateChildProcess(LPSTR strCmdLine, HANDLE& hProcess, HANDLE& hPipe)
{ 

	HANDLE hSaveStdout;
	HANDLE hChildStdoutRd;
	HANDLE hChildStdoutWr;
	//HANDLE hChildStdoutRdDup;
	SECURITY_ATTRIBUTES saAttr; 
	BOOL fSuccess; 

	// Set the bInheritHandle flag so pipe handles are inherited. 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

	// The steps for redirecting child process's STDOUT: 
	//     1. Save current STDOUT, to be restored later. 
	//     2. Create anonymous pipe to be STDOUT for child process. 
	//     3. Set STDOUT of the parent process to be write handle to 
	//        the pipe, so it is inherited by the child process. 
	//     4. Create a noninheritable duplicate of the read handle and
	//        close the inheritable read handle. 

	// Save the handle to the current STDOUT. 

	hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE); 

	// Create a pipe for the child process's STDOUT. 

	if (! CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) 
	  return FALSE; 

	// Set a write handle to the pipe to be STDOUT. 

	SetStdHandle(STD_OUTPUT_HANDLE, hChildStdoutWr);

	// Create noninheritable read handle and close the inheritable read 
	// handle. 

	fSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdoutRd,
		GetCurrentProcess(), &hPipe , 0,
		FALSE,
		DUPLICATE_SAME_ACCESS);

	if( !fSuccess )
		return FALSE;

	CloseHandle(hChildStdoutRd);


// After process creation, restore the saved STDIN and STDOUT. 



   PROCESS_INFORMATION piProcInfo; 
   STARTUPINFO siStartInfo; 

// Set up members of the PROCESS_INFORMATION structure. 

   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

// Set up members of the STARTUPINFO structure. 

   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO); 

// Create the child process. 

   fSuccess = CreateProcess(NULL, 
	  strCmdLine,       // command line 
	  NULL,          // process security attributes 
	  NULL,          // primary thread security attributes 
	  TRUE,          // handles are inherited 
	  0,             // creation flags 
	  NULL,          // use parent's environment 
	  NULL,          // use parent's current directory 
	  &siStartInfo,  // STARTUPINFO pointer 
	  &piProcInfo);  // receives PROCESS_INFORMATION 

	if( !fSuccess )
		return FALSE;

	hProcess = piProcInfo.hProcess;

	SetStdHandle(STD_OUTPUT_HANDLE, hSaveStdout);

	CloseHandle(hChildStdoutWr);

	return TRUE;
}

ClassNode* MaFenetre::GetClassNode(MdzString strName)
{
	MdzTreeNode* pRootNode = treeCtrl.GetRootNode();

	for(int i=0; i < (int)pRootNode->GetChildCount(); i++)
	{
		MdzTreeNode* pNode = pRootNode->GetChildAt(i);
		if (pNode->GetText().Equals(strName))
			return (ClassNode*)pNode;
	}
	return NULL;
}

void MaFenetre::methodNode_OnFocus(UINT bGotFocus)
{
	if (bGotFocus)
	{
		MdzTextArea* pTextArea = ((MethodNode*)pSelNode)->GetTextArea();
		BOOL bTextSelected = (pTextArea->GetSelText().GetLength() > 0);
		copyBtn.SetEnabled(bTextSelected);
		cutBtn.SetEnabled(bTextSelected);
		pasteBtn.SetEnabled(pTextArea->CanPaste());
		copyItem.SetEnabled(bTextSelected);
		cutItem.SetEnabled(bTextSelected);
		findItem.SetEnabled(TRUE);
		findBtn.SetEnabled(TRUE);
		pasteItem.SetEnabled(pTextArea->CanPaste());
	}
	else
	{
		//copyBtn.SetEnabled(FALSE);
		cutBtn.SetEnabled(FALSE);
		pasteBtn.SetEnabled(FALSE);
		copyBtn.SetEnabled(FALSE);
		copyItem.SetEnabled(FALSE);
		cutItem.SetEnabled(FALSE);
		pasteItem.SetEnabled(FALSE);
		findItem.SetEnabled(FALSE);
		findBtn.SetEnabled(FALSE);
	}
}

void MaFenetre::methodNode_OnSelChange()
{
	MdzTextArea* pTextArea = ((MethodNode*)pSelNode)->GetTextArea();
	if (!pTextArea->HasFocus())
		return;
	BOOL bTextSelected = (pTextArea->GetSelText().GetLength() > 0);
	copyBtn.SetEnabled(bTextSelected);
	cutBtn.SetEnabled(bTextSelected);
	copyItem.SetEnabled(bTextSelected);
	cutItem.SetEnabled(bTextSelected);
}


BOOL MaFenetre::GetBitmapId(MdzString& strInitValue)
{
	return dlgResource.GetBitmapId(strInitValue);
}

void MaFenetre::Init(BOOL bEnabled)
{
	pSelNode = NULL;
	//buildList.SetText("");
	buildList.RemoveAllItems();

	copyItem.SetEnabled(FALSE);
	cutItem.SetEnabled(FALSE);
	pasteItem.SetEnabled(FALSE);
	copyBtn.SetEnabled(FALSE);
	cutBtn.SetEnabled(FALSE);
	pasteBtn.SetEnabled(FALSE);
	findItem.SetEnabled(FALSE);
	findBtn.SetEnabled(FALSE);

	deleteBtn.SetEnabled(FALSE);
	moveupBtn.SetEnabled(FALSE);
	movedownBtn.SetEnabled(FALSE);

	resourceItem.SetEnabled(bEnabled);
	buildItem.SetEnabled(bEnabled);
	runItem.SetEnabled(bEnabled);
	closeItem.SetEnabled(bEnabled);
	saveItem.SetEnabled(bEnabled);
	buildBtn.SetEnabled(bEnabled);
	runBtn.SetEnabled(bEnabled);
	saveBtn.SetEnabled(bEnabled);

	//dlgResource.Init();
}

void MaFenetre::OnHelp()
{
	MyNode* pNode = (MyNode*)treeCtrl.GetSelNode();
	if (pNode)
	{
		//if (pNode->GetClassName().Equals("MethodNode") || pNode->GetClassName().Equals("CallbackNode"))
		if (pNode->IsInstanceOf("MethodNode"))
		{
			BOOL found = FALSE;

			MdzTextArea* pTextArea = ((MethodNode*)pNode)->GetTextArea();
			if (pTextArea->GetSelText().GetLength())
			{
#ifdef DEV
				OpenHtmlHelpFile(MdzSystem::GetExePath() + MdzString("\\..\\wtk\\doc\\api_doc.chm"), pTextArea->GetSelText());
#else
				OpenHtmlHelpFile(MdzSystem::GetExePath() + MdzString("\\wtk\\doc\\api_doc.chm"), pTextArea->GetSelText());
#endif
			}


		}	
	}
	
}

MaFenetre::MaFenetre()
{
	hProcess = NULL;
	bIsRegistered = FALSE;

	MdzSize sz = MdzSystem::GetScreenSize();
	//SetSize(sz.cx, sz.cy);

	stopBtn.SetEnabled(FALSE);

	debugThread.SetOnRun(MDZ_CBK1(MaFenetre, debugThread_OnRun));
	Init(FALSE);

	SetTitle(APPNAME);
	SetIcon(IDI_ICON1);
	propPanel.SetBackColor(MdzColor::GetSysColor());
	propPanel.SetBorder(TRUE);
	propPanel.SetOnSize(MDZ_CBK2(MaFenetre, propPanel_OnSize));
	
	SetDockBar(dockBar);
	Add(divPanel);
	Add(tabCtrl);
	SetBackColor(MdzColor::GetSysColor());
	tabCtrl.AddTab("Build", buildList);
	tabCtrl.AddTab("Console", debugText);
	divPanel.SetLeftCtrl(leftPanel, TREEWIDTH);
	divPanel.SetRightCtrl(propPanel);
	//divPanel.SetLeftCtrl(treeCtrl, TREEWIDTH);
	leftPanel.Add(treeCtrl);
	leftPanel.Add(toolBar2);
	leftPanel.SetOnSize(MDZ_CBK2(MaFenetre, leftPanel_OnSize));
	leftPanel.SetBorder(TRUE);


	dockBar.AddToolBar(toolBar);
	dockBar.AddToolBar(toolBar3);
	//dockBar.AddToolBar(toolBar2, TRUE);

	AddShortcut(0, MdzKey::K_F1, MDZ_CBK1(MaFenetre, OnHelp));
	AddShortcut(MdzKey::K_CTRL, ' ', MDZ_CBK1(MaFenetre, OnDisplayVarList));

	toolBar.AddButton(newBtn, MdzBitmap(IDB_NEW));
	toolBar.AddButton(openBtn, MdzBitmap(IDB_OPEN));
	toolBar.AddButton(saveBtn, MdzBitmap(IDB_SAVE));
	toolBar.AddSeparator();
	toolBar.AddButton(buildBtn, MdzBitmap(IDB_BUILD));
	toolBar.AddButton(runBtn, MdzBitmap(IDB_DEBUG));
	toolBar.AddButton(stopBtn, MdzBitmap(IDB_STOP));

	newBtn.SetToolTips("Create New Project");
	openBtn.SetToolTips("Open Project");
	saveBtn.SetToolTips("Save Project");
	buildBtn.SetToolTips("Build Project");
	runBtn.SetToolTips("Run");
	stopBtn.SetToolTips("Stop");

	newBtn.SetOnClick(MDZ_CBK1(MaFenetre, newItem_OnClick));
	openBtn.SetOnClick(MDZ_CBK1(MaFenetre, openItem_OnClick));
	saveBtn.SetOnClick(MDZ_CBK1(MaFenetre, saveItem_OnClick));
	buildBtn.SetOnClick(MDZ_CBK1(MaFenetre, buildItem_OnClick));
	runBtn.SetOnClick(MDZ_CBK1(MaFenetre, runItem_OnClick));
	stopBtn.SetOnClick(MDZ_CBK1(MaFenetre, stopItem_OnClick));

	toolBar2.AddButton(deleteBtn, MdzBitmap(IDB_DELETE));
	toolBar2.AddButton(moveupBtn, MdzBitmap(IDB_MOVEUP));
	toolBar2.AddButton(movedownBtn, MdzBitmap(IDB_MOVEDOWN));

	deleteBtn.SetToolTips("Delete selected node");
	moveupBtn.SetToolTips("MoveUp selected node");
	movedownBtn.SetToolTips("MoveDown selected node");
	
	deleteBtn.SetOnClick(MDZ_CBK1(MaFenetre, deleteBtn_OnClick));
	moveupBtn.SetOnClick(MDZ_CBK1(MaFenetre, moveupBtn_OnClick));
	movedownBtn.SetOnClick(MDZ_CBK1(MaFenetre, movedownBtn_OnClick));

	toolBar3.AddButton(cutBtn, MdzBitmap(IDB_CUT));
	toolBar3.AddButton(copyBtn, MdzBitmap(IDB_COPY));
	toolBar3.AddButton(pasteBtn, MdzBitmap(IDB_PASTE));
	toolBar3.AddSeparator();
	toolBar3.AddButton(findBtn, MdzBitmap(IDB_FIND));

	cutBtn.SetToolTips("Cut");
	copyBtn.SetToolTips("Copy");
	pasteBtn.SetToolTips("Paste");
	findBtn.SetToolTips("Find");

	cutBtn.SetOnClick(MDZ_CBK1(MaFenetre, cutItem_OnClick));
	copyBtn.SetOnClick(MDZ_CBK1(MaFenetre, copyItem_OnClick));
	pasteBtn.SetOnClick(MDZ_CBK1(MaFenetre, pasteItem_OnClick));
	findBtn.SetOnClick(MDZ_CBK1(MaFenetre, findItem_OnClick));


	treeCtrl.SetOnSelChange(MDZ_CBK1(MaFenetre, treeCtrl_OnSelChange));
	treeCtrl.SetOnRightClick(MDZ_CBK2(MaFenetre, treeCtrl_OnRightClick));

	buildList.SetOnDblClick(MDZ_CBK1(MaFenetre, listbox_OnDblClick));
	buildList.SetFont(MdzFont("Courier New", 10));
	InitMenu();
	Init(FALSE);

}

void MaFenetre::InitMenu()
{
	SetMenuBar(menuBar);
	menuBar.Add(fileMenu, "File");
	menuBar.Add(projectMenu, "Project");
	menuBar.Add(editMenu, "Edit");
	menuBar.Add(helpMenu, "Help");

	buildItem.SetBitmap(IDB_BUILD);
	runItem.SetBitmap(IDB_DEBUG);
	openItem.SetBitmap(IDB_OPEN);
	newItem.SetBitmap(IDB_NEW);
	saveItem.SetBitmap(IDB_SAVE);
	copyItem.SetBitmap(IDB_COPY);
	pasteItem.SetBitmap(IDB_PASTE);
	cutItem.SetBitmap(IDB_CUT);
	findItem.SetBitmap(IDB_FIND);


	projectMenu.Add(buildItem, "Build", MDZ_CBK1(MaFenetre, buildItem_OnClick));
	projectMenu.Add(runItem, "Run", MDZ_CBK1(MaFenetre, runItem_OnClick));
	projectMenu.AddSeparator();
	projectMenu.Add(resourceItem, "Resource...", MDZ_CBK1(MaFenetre, resourceItem_OnClick));


	fileMenu.Add(newItem, "New...", MDZ_CBK1(MaFenetre, newItem_OnClick));
	fileMenu.Add(openItem, "Open...", MDZ_CBK1(MaFenetre, openItem_OnClick));
	fileMenu.Add(closeItem, "Close", MDZ_CBK1(MaFenetre, closeItem_OnClick));
	fileMenu.AddSeparator();
	fileMenu.Add(saveItem, "Save", MDZ_CBK1(MaFenetre, saveItem_OnClick));
	fileMenu.AddSeparator();
	fileMenu.AddItem("Exit", MDZ_CBK1(MaFenetre, exitItem_OnClick));

	editMenu.Add(cutItem, "Cut\tCtrl+X", MDZ_CBK1(MaFenetre, cutItem_OnClick));
	editMenu.Add(copyItem, "Copy\tCtrl+C", MDZ_CBK1(MaFenetre, copyItem_OnClick));
	editMenu.Add(pasteItem, "Paste\tCtrl+V", MDZ_CBK1(MaFenetre, pasteItem_OnClick));
	editMenu.AddSeparator();
	editMenu.Add(findItem, "Find", MDZ_CBK1(MaFenetre, findItem_OnClick));
	//editMenu.SetOnClick(MDZ_CBK1(MaFenetre, editMenu_OnClick));

	helpMenu.Add(registerItem, "Register...", MDZ_CBK1(MaFenetre, registerItem_OnClick));
	helpMenu.Add(keyItem, "Enter register key...", MDZ_CBK1(MaFenetre, keyItem_OnClick));
	helpMenu.AddSeparator();
	helpMenu.AddItem("WTK API Help index...", MDZ_CBK1(MaFenetre, apiItem_OnClick));
	helpMenu.AddSeparator();
	helpMenu.AddItem("About...", MDZ_CBK1(MaFenetre, aboutItem_OnClick));
	helpMenu.SetOnClick(MDZ_CBK1(MaFenetre, helpMenu_OnClick));
	findItem.SetShortcut(MdzKey::K_CTRL, 'H');
	buildItem.SetShortcut(0, MdzKey::K_F7);
	runItem.SetShortcut(0, MdzKey::K_F5);
	openItem.SetShortcut(MdzKey::K_CTRL, 'O');
	newItem.SetShortcut(MdzKey::K_CTRL, 'N');
	saveItem.SetShortcut(MdzKey::K_CTRL, 'S');

}

void MaFenetre::apiItem_OnClick()
{
	OpenHtmlHelpFile(MdzSystem::GetExePath() + MdzString("\\wtk\\doc\\api_doc.chm"));
}

void MaFenetre::aboutItem_OnClick()
{		
	MdzString strAppName = APPNAME;
	if (!bIsRegistered)
		strAppName += MdzString(" (Trial)");

	MdzString strMsg = MdzString::Format(
		"%s\n\n"
		"Product ID:\t%s\n\n"
		"Author:\t\tMarc DELOMEZ\n"
		"Version:\t\t%s\n"
		"Build Date:\t%s\n"
		"Email:\t\tmarc.delomez@free.fr\n"
		"Home Page:\thttp://marc.delomez.free.fr/ide\n",
		(char*)strAppName, (char*)strProductId, VERSION, __DATE__);

	MSGBOXPARAMS params;
	ZeroMemory(&params, sizeof(params));
	params.cbSize = sizeof(params);
	params.hwndOwner = GetHandle();
	params.dwStyle = MB_OK | MB_USERICON;
	params.lpszCaption = "About";
	params.lpszText = strMsg;
	params.hInstance = GetModuleHandle(NULL);
	params.lpszIcon = MAKEINTRESOURCE(IDI_ICON1);

	MessageBoxIndirect(&params);

}

void MaFenetre::moveupBtn_OnClick()
{
	pSelNode->MoveUp();
}

void MaFenetre::movedownBtn_OnClick()
{
	pSelNode->MoveDown();
}

void MaFenetre::resourceItem_OnClick()
{
	dlgResource.Execute();
}

void MaFenetre::deleteBtn_OnClick()
{
	if (pSelNode)
	{			
		int ret = MdzStdDialog::ShowConfirm("Are you sure ?", "Delete");
		if (ret != (int)MdzStdDialog::K_YES)
			return;
		MyNode* pNode = pSelNode;
		pSelNode = (MyNode*)pNode->GetParent();
		pNode->Remove();
		delete pNode;
		pSelNode->Select();
	}
}

void MaFenetre::cutItem_OnClick() 
{
	MdzTextArea* pTextArea = ((MethodNode*)treeCtrl.GetSelNode())->GetTextArea();
	pTextArea->Cut();
}

void MaFenetre::copyItem_OnClick() 
{
	MyNode* pNode = (MyNode*)treeCtrl.GetSelNode();
	if (!pNode)
		return;
	if (pNode->IsInstanceOf("MethodNode"))
	{
		MdzTextArea* pTextArea = ((MethodNode*)pNode)->GetTextArea();
		if (pTextArea->HasFocus())
		{
			pTextArea->Copy();
			return;
		}
	}
}

void MaFenetre::pasteItem_OnClick() 
{		
	MdzTextArea* pTextArea = ((MethodNode*)treeCtrl.GetSelNode())->GetTextArea();
	pTextArea->Paste();
}


void MaFenetre::editMenu_OnClick()
{
	copyItem.SetEnabled(FALSE);
	cutItem.SetEnabled(FALSE);
	pasteItem.SetEnabled(FALSE);
	//MyNode* pNode = treeCtrl.GetSelNode();
	if (!pSelNode)
		return;

	//MdzString strClass = MdzString(pSelNode->GetClassName());
	if (pSelNode->GetClassName().Equals("MethodNode") || pSelNode->GetClassName().Equals("CallbackNode"))
	{
		MdzTextArea* pTextArea = ((MethodNode*)pSelNode)->GetTextArea();
		if (!pTextArea->HasFocus())
			return;

		BOOL bTextSelected = (pTextArea->GetSelText().GetLength() > 0);

		copyItem.SetEnabled(bTextSelected);
		cutItem.SetEnabled(bTextSelected);
		pasteItem.SetEnabled(pTextArea->CanPaste());
	}
}



void MaFenetre::AddToolButton(MdzToolBar& toolbar, UINT iBmpIdx, MdzCbk1 *pFunc)
{
	MdzToolButton* pBtn = new MdzToolButton;
	pBtn->SetAutoDelete(TRUE);
	pBtn->SetOnClick(pFunc);
	toolbar.AddButton(*pBtn, iBmpIdx);
}

void MaFenetre::AddToolButton(MdzToolBar& toolbar, MdzBitmap bmp, MdzCbk1 *pFunc)
{
	MdzToolButton* pBtn = new MdzToolButton;
	pBtn->SetAutoDelete(TRUE);
	pBtn->SetOnClick(pFunc);
	toolBar.AddButton(*pBtn, bmp);
}

void MaFenetre::SetDebug(BOOL bEnabled)
{
	runBtn.SetEnabled(!bEnabled);
	runItem.SetEnabled(!bEnabled);
	newBtn.SetEnabled(!bEnabled);
	newItem.SetEnabled(!bEnabled);
	openItem.SetEnabled(!bEnabled);
	openBtn.SetEnabled(!bEnabled);
	buildItem.SetEnabled(!bEnabled);
	buildBtn.SetEnabled(!bEnabled);
	stopBtn.SetEnabled(bEnabled);
}

void MaFenetre::stopItem_OnClick()
{
	if (hProcess)
		TerminateProcess(hProcess, 0);
}

void MaFenetre::runItem_OnClick()
{
	if (treeCtrl.GetRootNode())
	{
		debugThread.Start();

	}

}

void MaFenetre::debugThread_OnRun()
{
	HANDLE hPipe;
	CHAR chBuf[1024]; 
	DWORD dwRead;

	SetCurrentDirectory(strPathName);
	CreateChildProcess(strProjectName + MdzString(".exe"), hProcess, hPipe);
	if (!hProcess)
	{
		MdzStdDialog::ShowMessage("Project not build!");
		return;
	}

	debugText.SetText("");
	tabCtrl.SetSelIndex(1);
	SetDebug(TRUE);

	for(;;)
	{
		if( !ReadFile( hPipe, chBuf, sizeof(chBuf), &dwRead, NULL) || dwRead == 0) 
			break; 
		chBuf[dwRead] = 0;
		debugText.Append(chBuf);
		debugText.Append("\r\n");
		debugText.ScrollToEnd();
	}

	SetDebug(FALSE);

}

BOOL MaFenetre::OnClose()
{
	SaveProject();
	return TRUE;
}

void MaFenetre::closeItem_OnClick()
{
	SaveProject();
	Init(FALSE);
	SetTitle(APPNAME);
	treeCtrl.SetRootNode(NULL);
}

void MaFenetre::newItem_OnClick()
{
	SaveProject();

	DlgProject dlg;
	if (dlg.Execute())
	{
		strProjectName = dlg.GetName();
		strPathName = MdzString::Format("%s\\%s", (char*)dlg.GetPath(), (char*)strProjectName);
		if (!CreateDirectory(strPathName, NULL))
		{
			MdzStdDialog::ShowMessage(MdzString::Format("Directory %s already exists !", (char*)strPathName));
			return;
		}
		Init(TRUE);
		dlgResource.Init();
		SetTitle(MdzString::Format("%s - [%s]", APPNAME, (char*)strProjectName));
		treeCtrl.SetRootNode(RootNode::Create());
		//SaveProject(FALSE);
	}
}

void MaFenetre::openItem_OnClick()
{
	SaveProject();

	MdzFileDialog dlg;
	dlg.AddFilter("project", "prj");
#ifdef DEV
	dlg.SetInitDirectory(MdzSystem::GetExePath() + MdzString("\\..\\samples"));
#else
	dlg.SetInitDirectory(MdzSystem::GetExePath() + MdzString("\\samples"));
#endif
	if (dlg.ShowOpen())
	{
		if (LoadProject(dlg.GetFullPathName()))
		{
			Init(TRUE);
			strProjectName = dlg.GetFileName();
			strProjectName = strProjectName.GetSubString(0, strProjectName.GetIndexOf("."));
			strPathName = dlg.GetCurrentDirectory();
			SetTitle(MdzString::Format("%s - [%s]", APPNAME, (char*)strProjectName));
		}
	}
}






void MaFenetre::OnCreate()
{
	MdzFrame::OnCreate();

	/*
	strMacAddr = GetMacAddress();
	if (strMacAddr.GetLength() == 0)
	{
		MdzStdDialog::ShowMessage("Cannot read Product ID", APPNAME);
		Close();
		return;
	}
	*/
	MdzStrVector listMacAddr;
	GetMacAddress(listMacAddr);
	if (listMacAddr.GetCount() == 0)
	{
		MdzStdDialog::ShowMessage("Cannot read Product ID", APPNAME);
		Close();
		return;
	}

	MdzString strSavedKey;
	MdzBaseRegister::GetStringValue("Software\\MDZ IDE", "Key", strSavedKey);
	for(UINT i = 0; i < listMacAddr.GetCount(); i++)
	{
		if ( CheckKey(strSavedKey, listMacAddr[i]))
		{
			bIsRegistered = TRUE;
			break;
		}
	}

	strProductId = (bIsRegistered) ? listMacAddr[i] : listMacAddr[0];

	if (!dbClass.LoadDatabase(MdzSystem::GetExePath()))
	{
		MdzStdDialog::ShowMessage("Cannot load class database", "Warning");
	}
	
	MdzString strCmdLine = GetCmdLine();

	if (strCmdLine.GetLength() == 0)
		return;

	if (strCmdLine.GetCharAt(0) == '"')
		strCmdLine = strCmdLine.GetSubString(1, strCmdLine.GetLength()-2);

	//MdzStdDialog::ShowMessage(strCmdLine);


	if (!LoadProject(strCmdLine))
		return;
	Init(TRUE);
	strProjectName = strCmdLine.ExtractFileName();
	strProjectName = strProjectName.GetSubString(0, strProjectName.GetIndexOf("."));
	strPathName = strCmdLine.ExtractPath();
	SetTitle(MdzString::Format("%s - [%s]", APPNAME, (char*)strProjectName));
}

void MaFenetre::saveItem_OnClick()
{
	SaveProject(FALSE);
}

void MaFenetre::SaveNode(MdzXmlWriter& writer, MyNode* pNode)
{
	writer.WriteStartElement(pNode->GetClassName());
	pNode->Save(writer);
	for(int i = 0; i < (int)pNode->GetChildCount(); i++)
		SaveNode(writer, (MyNode*)pNode->GetChildAt(i));
	writer.WriteEndElement();
}

BOOL MaFenetre::LoadNode(MdzXmlElement* pElt, MyNode* pParentNode)
{
	if (!pElt)
		return FALSE;

	MyNode* pNode = (MyNode*)MdzClassFactory::CreateObject(pElt->GetName());
	if (!pNode)
	{
		MdzStdDialog::ShowMessage(MdzString::Format("No factory for class %s",
			(char*)pElt->GetName()));
		return FALSE;
	}

	if (pParentNode)
		pParentNode->Add(pNode);
	else
		treeCtrl.SetRootNode(pNode);

	pNode->Load(pElt);
	pNode->InitProps();
	pNode->LoadProps(pElt);

	for(int i = 0; i < (int)pElt->GetChildCount(); i++)
	{
		BOOL bRet = LoadNode(pElt->GetChildAt(i), pNode);
		if (!bRet)
			return FALSE;
	}



	return TRUE;
}

void MaFenetre::SaveProject(BOOL bAskConfirm)
{

	if (treeCtrl.GetRootNode())
	{
		if (!bAskConfirm ||
			MdzStdDialog::ShowConfirm("Do you want to save your modifications ?", APPNAME) == MdzStdDialog::K_YES)
		{
			/*
			if (!bIsRegistered)
			{
				 MdzStdDialog::ShowMessage("This feature is not available in the trial version.", "Save");
				return;
			}
			*/
			MdzString strName = MdzString::Format("%s\\%s.prj", (char*)strPathName, (char*)strProjectName);
			MdzXmlWriter writer(strName);
			SaveNode(writer, (MyNode*)treeCtrl.GetRootNode());
			dlgResource.Save(writer);
		}
	}

}

BOOL MaFenetre::LoadProject(MdzString projectName)
{
	MdzXmlParser parser;
	if (!parser.Open(projectName))
	{
		MdzStdDialog::ShowMessage(MdzString::Format("Cannot open file %s", projectName));
		return FALSE;
	}
	MdzXmlElement* pElt = parser.Parse();
	if (!pElt)
	{
		MdzStdDialog::ShowMessage("Bad file format!");
		return FALSE;
	}
			
	dlgResource.Load(projectName.ExtractPath(), pElt->GetChildByName("RESOURCES"));
	LoadNode(pElt->GetChildByName("RootNode"), NULL);
	
	MdzTreeNode* pRootNode = treeCtrl.GetRootNode();
	for(UINT i = 0; i < pRootNode->GetChildCount(); i++)
		pRootNode->GetChildAt(i)->SortChildren();

	resourceItem.SetEnabled(TRUE);
	return TRUE;
}

MethodNode* MaFenetre::FindMethodNode(int iLineNumber)
{
	MyNode* pNode = (MyNode*)treeCtrl.GetRootNode();
	while (pNode)
	{
		if (pNode->IsInstanceOf("MethodNode"))
		{
			MethodNode* pMethodNode = (MethodNode*)pNode;
			if (iLineNumber >= pMethodNode->GetStartLine() &&
				iLineNumber <= pMethodNode->GetEndLine())
				return pMethodNode;
		}
		pNode = (MyNode*)pNode->GetNext();
	}

	return NULL;

}
void MaFenetre::buildItem_OnClick()
{
	tabCtrl.SetSelIndex(0);
	BuildProject();
}

void MaFenetre::BuildProject()
{
	MyNode* pRootNode = (MyNode*)treeCtrl.GetRootNode();
	if (!pRootNode)
		return;
	
	buildList.RemoveAllItems();
	buildList.AddItem("Building application...");
	buildList.Update();

	SetCurrentDirectory(strPathName);

	OutputStream stream;
	stream.Open("code.cpp");
	stream.WriteLn("#include \"mdz_wtk.h\"");
	stream.WriteLn("#include \"resource.h\"");
	stream.WriteLn();
	for(int i = 0; i < (int)pRootNode->GetChildCount(); i++)
	{
		stream.WriteLn("class %s;", (char*)pRootNode->GetChildAt(i)->GetText());
	}
	stream.WriteLn();

	pRootNode->EncodeHead(stream);
	pRootNode->EncodeImp(stream);
	stream.Close();

	stream.Open("resource.h");
	dlgResource.WriteHeader(stream);
	stream.Close();

	FILE* pFile = fopen("code.rc", "w");
	fprintf(pFile, "#include \"resource.h\"\n\n");
	fprintf(pFile, "1 24 manifest.xml\n");
	dlgResource.WriteScript(pFile);
	fclose(pFile);

	MdzString strExePath = MdzSystem::GetShortPath(MdzSystem::GetExePath());


	pFile = fopen("code.bat", "w");
#ifdef DEV
	fprintf(pFile, "set MDZ_WTK_PATH=\"%s\\..\\wtk\"\n", (LPSTR)strExePath);
	fprintf(pFile, "set VC_PATH=\"%s\\..\\compilo\"\n", (LPSTR)strExePath);
	//MdzString strCmd = MdzString::Format("%s\\..\\compilo\\handle.exe %s.exe -p %s > dump.txt\n", (char*)strExePath, (char*)strProjectName, (char*)MdzSystem::GetExeName());
	//MdzSystem::RunProcess(strCmd, TRUE);
#else
	fprintf(pFile, "set MDZ_WTK_PATH=\"%s\\wtk\"\n", (LPSTR)strExePath);
	fprintf(pFile, "set VC_PATH=\"%s\\compilo\"\n", (LPSTR)strExePath);
#endif

	fprintf(pFile, "copy %%MDZ_WTK_PATH%%\\lib\\mdz_wtk.dll .\n");
	fprintf(pFile, "copy %%VC_PATH%%\\Bin\\manifest.xml .\n");
	fprintf(pFile, "set LIB=%%VC_PATH%%\\lib;%%MDZ_WTK_PATH%%\\lib\n");
#ifdef DEV
	fprintf(pFile, "%%VC_PATH%%\\Bin\\cl /c /I%%VC_PATH%%\\include /I%%MDZ_WTK_PATH%% code.cpp\n");
#else
	fprintf(pFile, "%%VC_PATH%%\\Bin\\cl /c /I%%VC_PATH%%\\include /I%%MDZ_WTK_PATH%%\\include code.cpp\n");
#endif
	fprintf(pFile, "%%VC_PATH%%\\bin\\rc code.rc\n");
	fprintf(pFile, "%%VC_PATH%%\\bin\\link /OUT:%s.exe code.obj code.res mdz_wtk.lib\n", (char*)strProjectName);
	fclose(pFile);

	MdzSystem::RunProcess("code.bat > code.txt", TRUE);	


	buildList.RemoveAllItems();
	pFile = fopen("code.txt", "r");
	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), pFile))
	{
		int iLine;
		int ret = sscanf(buffer, "code.cpp(%d)", &iLine);

		if (ret == 1)
		{
			iLine--;
			char* p = strtok(buffer, ":");
			char* p2 = strtok(NULL, ":");
			char* p3 = strtok(NULL, "\n");
			
			char* p4 = strstr(p3, " before '}'");
			if (p4)
			{	
				MethodNode* pNode = FindMethodNode(iLine);
				if (pNode && pNode->GetEndLine() == iLine)
				{
					*p4 = 0;
					iLine--;
				}
			}
			
			buildList.AddItem(p3, (LPVOID)iLine);

		}
	}
	fclose(pFile);
	if (buildList.GetItemCount() == 0)
		buildList.AddItem("Application build successfully !");
#ifndef DEV 
	//DeleteFile("code.cpp");
	DeleteFile("code.bat");
	DeleteFile("code.obj");
	DeleteFile("code.txt");
	DeleteFile("code.rc");
	DeleteFile("code.res");
	DeleteFile("resource.h");
	DeleteFile("manifest.xml");
#endif

}

void MaFenetre::treeCtrl_OnSelChange()
{
	if (pSelNode)
		pSelNode->Unselect();
	pSelNode = (MyNode*)treeCtrl.GetSelNode();

	pSelNode->Activate(&propPanel);
	deleteBtn.SetEnabled(pSelNode->IsRemovable());
	moveupBtn.SetEnabled(pSelNode->CanMove() && !pSelNode->IsFirstChild());
	movedownBtn.SetEnabled(pSelNode->CanMove() && !pSelNode->IsLastChild());
	
	if (pSelNode->IsInstanceOf("MethodNode"))
	{
		((MethodNode*)pSelNode)->GetTextArea()->SetOnSelChange(MDZ_CBK1(MaFenetre, methodNode_OnSelChange));
		((MethodNode*)pSelNode)->GetTextArea()->SetOnFocus(MDZ_CBK6(MaFenetre, methodNode_OnFocus));
	}




}

void MaFenetre::treeCtrl_OnRightClick(int x, int y)
{
	MyNode* pNode = (MyNode*)treeCtrl.GetNodeAt(x, y);
	if (pNode)
	{
		pNode->GetCtxMenu()->Track(treeCtrl, x, y);
	}
}

void MaFenetre::panel_OnSize(int width, int height)
{
	OutputDebugString("MaFenetre::panel_OnSize\n");
	divPanel.SetBounds(5, 5, width-10, height-TABHEIGHT-10);
	tabCtrl.SetBounds(5, height-TABHEIGHT, width-10, TABHEIGHT-5);
	if (pSelNode)
		pSelNode->Activate(&propPanel);
}




void Prop::BtnBitmap_OnClick(LPVOID pArg)
{
	MdzTextField* pTextField = (MdzTextField*)pArg;
	MdzString strValue = pTextField->GetText();
	if (((MaFenetre*)MdzFrame::GetMainFrame())->GetBitmapId(strValue))
		pTextField->SetText(strValue);
}


void ClassNode::addInitDoneMethods()
{
	MethodNode* pNode = MethodNode::Create("Done", "", "void");
	pNode->SetRemovable(FALSE);
	AddNode(pNode);
	pNode = MethodNode::Create("Init", "", "void");
	pNode->SetRemovable(FALSE);
	AddNode(pNode);
}

void ClassNode::addMethod_OnClick()
{
	DlgMethod dlg;
	if (dlg.Execute())
		AddNode(MethodNode::Create(dlg.GetName(), dlg.GetProto(), dlg.GetRetValue(), dlg.GetType()));
}

void ClassNode::addVariable_OnClick()
{
	DlgVariable dlg;
	if (!dlg.Execute())
		return;

	MdzString varType = dlg.GetType();
	if (!varType.GetSubString(0, 3).Equals("Mdz") || dlg.GetArraySize() > 0)
	{
		AddNode(VarNode::Create(dlg.GetName(), varType, dlg.GetArraySize()));
		return;
	}

	MyNode* pNode = (MyNode*)MdzClassFactory::CreateObject(varType.GetSubString(3) + MdzString("Node"));
	if (!pNode)
	{
		AddNode(VarNode::Create(dlg.GetName(), varType));
		return;
	}

	pNode->SetText(dlg.GetName());
	AddNode(pNode);

}

void MethodNode::code_OnChar(UINT iChar)
{
	

	int startIdx = code.GetStartIndex();

	if (IsInQuote(startIdx))
	{
		code.ReplaceSelection(MdzString::Format("%c", iChar));
		return;
	}

	if (iChar == '\t')
	{
		MdzString strSelText = code.GetSelText();
		if (strSelText.GetLength() > 0)
		{
			int stopIdx = startIdx + strSelText.GetLength(); 
			if (! code.IsBeginOfLine(startIdx))
				return;
			if (! code.IsBeginOfLine(stopIdx) && !code.IsEndOfLine(stopIdx))
				return;
			BOOL bShiftDown = MdzKey::IsKeyDown(MdzKey::K_SHIFT);
			MdzStrVector vect;
			strSelText.Tokenize("\r\n", vect);
			MdzString str;
			for(int i = 0; i < (int)vect.GetCount(); i++)
			{
				if (i > 0) str += MdzString("\r\n");
				if (!bShiftDown)
					str += MdzString("\t") + vect[i];
				else if (vect[i].GetCharAt(0) == '\t')
					str += vect[i].GetSubString(1);
			}
			if (code.IsBeginOfLine(stopIdx))
				str += MdzString("\r\n");

			code.ReplaceSelection(str);
			code.SetSelection(startIdx, code.GetStartIndex());
			return;
		}
	}

	code.ReplaceSelection(MdzString::Format("%c", iChar));
	startIdx = code.GetStartIndex();

	MdzString strCode = code.GetText();


	if (iChar == '(')
	{
		code.ReplaceSelection(")");			
		code.SetSelection(startIdx, startIdx);
		return;
	}

	if (iChar == '[')
	{
		code.ReplaceSelection("]");			
		code.SetSelection(startIdx, startIdx);
		return;
	}

	if (iChar == '"')
	{
		code.ReplaceSelection("\"");			
		code.SetSelection(startIdx, startIdx);
		return;
	}

	if (iChar == '{')
	{
		int idx = code.GetLineFromIndex(startIdx);
		MdzString strLine = code.GetLineAt(idx);
		int nbTab = 0;
		int i = 0;
		while (strLine.GetCharAt(i++) == '\t')
			nbTab++;
		code.ReplaceSelection("\r\n");
		int saveIdx = code.GetStartIndex();
		code.ReplaceSelection("\r\n");
		for (i = 0; i < nbTab; i++)
			code.ReplaceSelection("\t");			
		code.ReplaceSelection("}");

		code.SetSelection(saveIdx, saveIdx);
		for (i = 0; i < nbTab+1; i++)
			code.ReplaceSelection("\t");			


		return;
	}


	startIdx -= 2;


	UINT iFound = 0;

	if (iChar == ':')
	{
		if (startIdx >= 0 && strCode.GetCharAt(startIdx) == ':')
		{
			startIdx--;
			iFound = 1;
		}
	}

	if (iChar == '.')
		iFound = 2;
	
	if (iChar == '>')
	{
		if (startIdx >= 0 && strCode.GetCharAt(startIdx) == '-')
		{
			startIdx--;
			iFound = 3;
		}
	}

	if (iFound == 0)
		return;
		
	MdzString strVarName = GetKeyWord(strCode, startIdx);

	if (strVarName.GetLength() == 0)
		return;
		
	MdzStrVector vector;

	if (iFound > 1)
	{
		ClassNode* pClassNode = ((MaFenetre*)MdzFrame::GetMainFrame())->GetClassNode(GetFatherClass());
		if (!pClassNode)
			return;
		MdzString strVar;

		if (strVarName.Equals("this"))
		{
			strVar = pClassNode->GetBaseClass();
		}
		else
		{
			BOOL bIsPointer;
			strVar = FindLocalVariable(strCode, strVarName, bIsPointer);
			if (iFound == 3 && !bIsPointer)
				return;
			if (iFound == 2 && bIsPointer)
				return;
		
			if (strVar.GetLength() == 0)
			{
				strVar = GetParameterType(strVarName);
				
				if (strVar.GetLength() == 0)
				{			
					VarNode* pVarNode = pClassNode->GetVarNode(strVarName);
					if (!pVarNode)
						return;
					strVar = pVarNode->GetType();
				}

				if (iFound == 3)
					strVar = strVar.GetSubString(0, strVar.GetLength()-1);
			}
		}

		dbClass.GetMethodList(strVar, vector, Database::METHOD_NORMAL | Database::METHOD_VIRTUAL, TRUE);
	}
	else
	{
		dbClass.GetMethodList(strVarName, vector, Database::METHOD_STATIC, FALSE);
	}


	if (vector.GetCount() == 0)
		return;

	MdzMenu menu;

	for(int i = 0; i < (int)vector.GetCount(); i++)
	{
		if (vector[i].Equals("-"))
		{
			if (i+1 < (int)vector.GetCount())
				menu.AddSeparator();
		}
		else
			menu.AddItem(vector[i]);
	}
	MdzPoint pt;
	if (!code.GetPosFromIndex(startIdx, pt))
		return;
	MdzMenuItem* pItem = menu.Track(code, pt.x, pt.y + 15);
	if (pItem)
	{
		code.ReplaceSelection(pItem->GetCaption());
		strCode = code.GetText();
		int idx = strCode.GetIndexOf("(", startIdx) + 1;
		int idx2 = strCode.GetIndexOf(")", idx);
		if (idx2 > idx)
			code.SetSelection(idx, idx2);
	}


}

#define FILE_DEVICE_PHYSICAL_NETCARD 0x00000017 
#define METHOD_OUT_DIRECT 2
#define FILE_ANY_ACCESS 0 
#define OID_802_3_CURRENT_ADDRESS           0x01010102

//#define CTL_CODE(DeviceType, Function, Method, Access) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
#define IOCTL_NDIS_QUERY_GLOBAL_STATS CTL_CODE(FILE_DEVICE_PHYSICAL_NETCARD, 0, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

void MaFenetre::GetMacAddress(MdzStrVector& vector)
{
	MdzStrVector vector2;
	MdzBaseRegister::EnumKey("Software\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards", vector2);

	for(UINT i = 0; i < vector2.GetCount(); i++)
	{

		MdzString strServiceName;
		if (!MdzBaseRegister::GetStringValue(
			MdzString("Software\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards\\") + vector2[i],
			"ServiceName", strServiceName))
			continue;

		MdzString strDescription;
		MdzBaseRegister::GetStringValue(
			MdzString("Software\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards\\") + vector2[i],
			"Description", strDescription);


		HANDLE hFile = CreateFile(MdzString("\\\\.\\") + strServiceName,
			GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, INVALID_HANDLE_VALUE);

		if (hFile == INVALID_HANDLE_VALUE)
			continue;

		ULONG OidCode = OID_802_3_CURRENT_ADDRESS;
		UCHAR OidData[1024];
		DWORD bytesReturned;
		
		if (DeviceIoControl(hFile, IOCTL_NDIS_QUERY_GLOBAL_STATS, &OidCode, 
								 sizeof(OidCode), OidData, sizeof(OidData), &bytesReturned, NULL))
		{
			if (bytesReturned == 6)
			{
				MdzString ret;
				for(UINT i = 0; i < 6; i++)
					ret += MdzString::Format("%02X", OidData[i]);
				vector.Add(ret);
			}
		}

		CloseHandle(hFile);
	}

}

void MaFenetre::registerItem_OnClick()
{
	DlgRegister dlg;
	dlg.strPrice = PRICE;
	dlg.strProductId = strProductId;
	dlg.Execute();

}

void MaFenetre::keyItem_OnClick()
{
	MdzString strKey = MdzStdDialog::ShowInput("Enter key:", "Register");
	if (strKey.GetLength() == 0)
		return;
	if (!CheckKey(strKey, strProductId))
	{
		MdzStdDialog::ShowMessage("Bad key!");
		return;
	}
	bIsRegistered = TRUE;

	MdzBaseRegister::SetStringValue("Software\\MDZ IDE", "Key", strKey);



}

BOOL MaFenetre::CheckKey(MdzString strKey, MdzString strProductId)
{
	MdzString strComputedKey = MdzCrypto::ComputePublicKey((LPBYTE)(LPSTR)strProductId, strProductId.GetLength(), "XFSB8YDG");
	return strComputedKey.Equals(strKey);
	
}

void MaFenetre::helpMenu_OnClick()
{
	registerItem.SetEnabled(!bIsRegistered);
	keyItem.SetEnabled(!bIsRegistered);
}

DlgRegister::DlgRegister()
{
	SetTitle("Register");
	MdzLayout layout(this, 10, 10);
	layout.Add(panel, 400, 150);
	layout.Add(btnBuy, 167, 46, MdzLayout::K_BOTTOM_RIGHT);
	panel.SetBorder(TRUE);
	Pack(10, 10);

	panel.SetOnPaint(MDZ_CBK4(DlgRegister, panel_OnPaint));
	btnBuy.SetOnClick(MDZ_CBK1(DlgRegister, btnBuy_OnClick));
	btnBuy.SetBitmap(IDB_PAYPAL);
	//btnBuy.SetToolTips();
}

void DlgRegister::panel_OnPaint(MdzGraphic &graphic)
{
	MdzString strText = MdzString::Format("MDZ IDE ne coute que %s euros.\n\n"
		"Le paiement s'effectue sur le site sécurisé Paypal.\n\n"
		"Une fois le paiement effectué, vous recevrez votre clé d'enregistrement par email dans les jours qui suivent.",
		(LPSTR) strPrice);

	MdzRect rect = GetClientRect();
	rect.Inflate(-10, -10);
	graphic.SetFont(MdzFont("Arial", 11));
	graphic.SetColor(MdzColor(0, 128, 192));
	graphic.DrawString(strText, rect);

}

void DlgRegister::btnBuy_OnClick()
{
	
	MdzString strCmd = MdzString::Format("\"https://www.paypal.com/cgi-bin/webscr ?cmd=_xclick&business=marc.delomez@free.fr&item_name=MDZ%%20IDE&amount=%s&currency_code=EUR&on0=ProductID&os0=%s\"",
		(LPSTR)strPrice,
		(LPSTR) strProductId);
	ShellExecute(GetHandle(), NULL,
		strCmd,
		NULL,
		"c:\\",
		SW_SHOWNORMAL);
	Close(TRUE);
	

}

MdzString MaFenetre::GetProductID()
{
	return strProductId;
}

MdzString MethodNode::GetKeyWord(MdzString strCode, int iIndex)
{
	int cur = iIndex;
	while (cur >= 0 && IsGoodChar(strCode.GetCharAt(cur)))
		cur--;

	return strCode.GetSubString(cur+1, iIndex-cur);
}



MdzString MethodNode::FindLocalVariable(MdzString strCode, MdzString strVarName, BOOL& bIsPointer)
{
	bIsPointer = FALSE;
	int idx = strCode.GetIndexOf(MdzString(" ") + strVarName + MdzString(";"));
	if (idx < 0)
		idx = strCode.GetIndexOf(MdzString(" ") + strVarName + MdzString(" = "));

	if (idx <= 0)
		return MdzString();
	
	if (strCode.GetCharAt(idx-1) == '*')
	{
		bIsPointer = TRUE;
		idx--;
	}
	
	MdzString strKeyWord = GetKeyWord(strCode, idx-1);
	if (!strKeyWord.Equals("delete"))
		return strKeyWord;

	idx = strCode.GetIndexOf(MdzString(" ") + strVarName + MdzString(" = "));

	if (idx <= 0)
		return MdzString();

	if (strCode.GetCharAt(idx-1) == '*')
	{
		bIsPointer = TRUE;
		idx--;
	}

	return GetKeyWord(strCode, idx-1);

}

void MaFenetre::findItem_OnClick()
{
	MyNode* pNode = (MyNode*)treeCtrl.GetSelNode();
	if (!pNode)
		return;
	if (pNode->IsInstanceOf("MethodNode"))
	{
		MdzTextArea* pTextArea = ((MethodNode*)pNode)->GetTextArea();
		if (pTextArea->HasFocus())
		{
			pTextArea->ShowFindDialog();
			return;
		}
	}	
}



void MethodNode::GetParameters()
{
	MdzStrVector vector1;
	proto.Tokenize(",", vector1);
	for (int i = 0; i < (int)vector1.GetCount(); i++)
	{
		MethodParameter* pParam = new MethodParameter;
		parameters.Add(pParam);
		MdzStrVector vector2;
		vector1[i].Tokenize(" ", vector2);
		pParam->strName = vector2[1];

		int len = vector2[0].GetLength();
		if (vector2[0].GetCharAt(len-1) == '&')
			pParam->strType = vector2[0].GetSubString(0, len-1);
		else
			pParam->strType = vector2[0];

	}
}

MethodNode::~MethodNode()
{
	parameters.RemoveAll(TRUE);
}

MdzString MethodNode::GetParameterType(MdzString strName)
{
	for(UINT i = 0; i < parameters.GetCount(); i++)
	{
		if (parameters[i].strName.Equals(strName))
			return parameters[i].strType;
	}

	return MdzString();
}

ClassNode::ClassNode()
{
	SetBitmap(IDB_CLASS);

}

MethodNode::MethodNode()
{
	SetBitmap(IDB_METHOD);

}



void DatabaseRecord::AddMethod(LPSTR strName, UINT iType)
{
	MethodInfo* pInfo = new MethodInfo;
	pInfo->iType = iType;
	pInfo->strName = strName;
	methods.Add(pInfo);
}

DatabaseRecord::~DatabaseRecord()
{
	methods.RemoveAll(TRUE);
}

int SortMethodFunc(MethodInfo* p1, MethodInfo* p2)
{
	return p1->strName.Compare(p2->strName);
}

BOOL Database::LoadDatabase(MdzString strPath)
{
#ifdef DEV
	FILE* pFile = fopen(strPath + MdzString("\\..\\class.db"), "r");
#else
	FILE* pFile = fopen(strPath + MdzString("\\wtk\\doc\\class.db"), "r");
#endif
	if (pFile == NULL)
		return FALSE;

	char buff[256];
	DatabaseRecord* pRecord = NULL;
	while (fgets(buff, sizeof(buff), pFile))
	{
		char* p = strchr(buff, 10);
		if (p)
			*p = 0;

		if (buff[0] == 0)
		{
			if (pRecord)
			{
				MdzSystem::SortVector(pRecord->methods, (MdzSortFunction)SortMethodFunc);
				pRecord = NULL;
			}
			continue;
		}

		if (!pRecord)
		{
			pRecord = new DatabaseRecord;
			records.Add(pRecord);
			pRecord->strName = strtok(buff, ":");
			char* p = strtok(NULL, ":");
			if (p)
				pRecord->strBaseName = p;
		}
		else
		{
			char* p = strstr(buff, "),");
			*(p+1) = 0;
			pRecord->AddMethod(buff+1, atoi(p+2));
		}				

	}
	fclose(pFile);

	return TRUE;
}

void Database::GetMethodList(MdzString strClass, MdzStrVector &vector, UINT iType, BOOL bRecursive)
{
	for(int i = 0; i < (int)records.GetCount(); i++)
	{
		DatabaseRecord* pClass = &records[i];
		if (pClass->strName.Equals(strClass))
		{
			for(int k = 0; k < (int)pClass->methods.GetCount(); k++)
			{
				if ((pClass->methods[k].iType & iType) != 0)
					vector.Add(pClass->methods[k].strName, TRUE);
			}
			if (bRecursive && pClass->strBaseName.GetLength() != 0)
			{
				vector.Add("-");
				GetMethodList(pClass->strBaseName, vector, iType, TRUE);
			}
			return;					
		}
	}
}

LayoutBorderNode::LayoutBorderNode()
{
	SetText("Border");
}

void LayoutBorderNode::EncodeConstructor(OutputStream &stream)
{
	MyNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	if (pParent->GetClassName().Equals("LayoutNode"))
	{
		MdzString strTitle = form.GetValue("Title");
		if (strTitle.GetLength() > 0)
		{
			stream.WriteLn("\t%s.AddBorder(%d, %s);", (LPSTR)pParent->GetRealName(),
				form.GetValue("Spacing").ToInt(), (LPSTR)STRING(strTitle));
		}
		else
		{
			stream.WriteLn("\t%s.AddBorder(%d);", (LPSTR)pParent->GetRealName(),
				form.GetValue("Spacing").ToInt());
		}

			
	}
}

void LayoutNode::addBorder_OnClick()
{
	AddNoCheck(new LayoutBorderNode);

}

void LayoutBorderNode::InitProps()
{
	form.AddProp(Prop::DIGIT, "Spacing", "10");
	form.AddProp(Prop::TEXT, "Title", "");
}

RadioButtonNode::RadioButtonNode()
{
	type = "MdzRadioButton";
	SetBitmap(IDB_RADIOBUTTON);
}

void RadioButtonNode::InitProps()
{
	CtrlNode::InitProps();
	form.SetValue("Height", "20");
	form.AddProp(Prop::FONT, "Font", "Microsoft Sans Serif,8,0");
	form.AddProp(Prop::TEXT, "Caption", "");
	form.AddProp(Prop::BOOL, "Checked", "0");
	AddMenuItem("Add OnClick", MDZ_CBK1(RadioButtonNode, addOnClick_OnClick));

}

void RadioButtonNode::addOnClick_OnClick()
{
	AddCallbackNode("OnClick", "", "void");

}

void RadioButtonNode::EncodeConstructor(OutputStream &stream)
{
	CtrlNode::EncodeConstructor(stream);
	AddMethodCall(stream, "SetFont", FONT(form.GetValue("Font")));
	AddMethodCall(stream, "SetCaption", STRING(form.GetValue("Caption")));
	AddMethodCall(stream, "SetChecked", form.GetValue("Checked"));

}

SliderNode::SliderNode()
{
	type = "MdzSlider";
}

TabCtrlNode::TabCtrlNode()
{
	type = "MdzTabCtrl";

}

TreeCtrlNode::TreeCtrlNode()
{
	type = "MdzTreeCtrl";

}

TableNode::TableNode()
{
	type = "MdzTable";

}

void SliderNode::InitProps()
{
	CtrlNode::InitProps();

	form.SetValue("Height", "25");
	form.AddProp(Prop::BOOL, "Border", "0");
	form.AddProp(Prop::DIGIT, "MinValue", "0");
	form.AddProp(Prop::DIGIT, "MaxValue", "100");
	form.AddProp(Prop::DIGIT, "InitValue", "0");

	AddMenuItem("Add OnSelChange", MDZ_CBK1(SliderNode, addOnSelChange_OnClick));

}

void SliderNode::EncodeConstructor(OutputStream &stream)
{
	CtrlNode::EncodeConstructor(stream);

	AddMethodCall(stream, "SetRange", PARAM2(form.GetValue("MinValue"),
		form.GetValue("MaxValue")));
	AddMethodCall(stream, "SetValue", form.GetValue("InitValue"));
	AddMethodCall(stream, "SetBorder", form.GetValue("Border"));

}

void SliderNode::addOnSelChange_OnClick()
{
	AddCallbackNode("OnSelChange", "", "void");

}





void CtrlNode::panel_OnSize(int width, int height)
{
	propertiesPane.SetBounds(0, 40, 330, height-40);

}

void CtrlNode::Activate(MdzCtrl *pCtrl)
{
	/*
	if (!panel.GetHandle())
	{
		MdzRect rc = form.GetBoundaryRect();
		rc.Inflate(20, 0);
		propertiesPane.SetVertScrollbar(rc.Height(), rc.Height()/10);
	}
	*/
	VarNode::Activate(pCtrl);
}



void Form::AddBorder()
{
	layout.AddBorder(20, "Properties");
}

void DlgProject::lstDirectory_OnDblClick()
{
	lblPath.SetText(GetPath());
}

void MyDialog::SetFocusCtrl(MdzCtrl &ctrl)
{
	pFocusCtrl = &ctrl;
}

CallbackNode::CallbackNode()
{
	SetBitmap(IDB_CALLBACK);
}

int ClassNode::OnCompareChild(MdzTreeNode *pNode1, MdzTreeNode *pNode2)
{
	MdzString strClassNode1 = pNode1->GetClass();
	MdzString strClassNode2 = pNode2->GetClass();

	if (!strClassNode1.Equals(strClassNode2))
	{
		if (strClassNode1.Equals("MethodNode"))
			return-1;
		
		if (strClassNode2.Equals("MethodNode"))
			return 1;

	}

	return MyNode::OnCompareChild(pNode1, pNode2);
}

void LayoutNode::AddControl(char *ctrlName, UINT iResId)
{
	MdzMenuItem* pItem = ctxMenu.AddItem(MdzString::Format("Add %s...", ctrlName) , MDZ_ARGCBK1(LayoutNode, ctrlName, addControl_OnClick));
	pItem->SetBitmap(iResId);
}

void MaFenetre::exitItem_OnClick()
{
	Close();
}

void SocketClientNode::InitProps()
{
	VarNode::InitProps();
	AddMenuItem("Add OnDataReceived", MDZ_CBK1(SocketClientNode, addOnDataReceived_OnClick));
	AddMenuItem("Add OnConnectionClosed", MDZ_CBK1(SocketClientNode, addOnConnectionClosed_OnClick));

}

void SocketClientNode::addOnDataReceived_OnClick()
{
	AddCallbackNode("OnDataReceived", "", "void");

}

void SocketClientNode::addOnConnectionClosed_OnClick()
{
	AddCallbackNode("OnConnectionClosed", "", "void");

}



void SocketServerNode::InitProps()
{
	VarNode::InitProps();
	AddMenuItem("Add OnIncomingConnection", MDZ_CBK1(SocketServerNode, addOnIncomingConnection_OnClick));
}

void SocketServerNode::addOnIncomingConnection_OnClick()
{
	AddCallbackNode("OnIncomingConnection", "", "void");

}

void TabPanelNode::InitProps()
{
	VarNode::InitProps();
	form.AddProp(Prop::TEXT, "Caption", "Tab");
	AddMenuItem("Add Layout...", MDZ_CBK1(TabPanelNode, addLayout_OnClick));
	AddMenuSeparator();
	AddMenuItem("Add OnSize", MDZ_CBK1(TabPanelNode, addOnSize_OnClick));
}

TabPanelNode* TabPanelNode::Create(char *name)
{
	TabPanelNode* pNode = new TabPanelNode;
	pNode->SetText(name);
	return pNode;

}

void TabPanelNode::EncodeConstructor(OutputStream &stream)
{
	VarNode::EncodeConstructor(stream);
	MyNode* pParent = (MyNode*)GetParent();
	stream.WriteLn("\t%s.AddTab(%s, %s);", (char*)pParent->GetRealName(),
		(char*)STRING(form.GetValue("Caption")),
		(char*)GetRealName());
	AddMethodCall(stream, "SetBackColor", "MdzColor::GetSysColor()");

}

void TabCtrlNode::InitProps()
{
	CtrlNode::InitProps();
	AddMenuItem("Add Tab...", MDZ_CBK1(TabCtrlNode, addTab_OnClick));
}

void TabCtrlNode::addTab_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add Tab");
	if (strName.GetLength())
		AddNode(TabPanelNode::Create(strName));

}

void TabPanelNode::addLayout_OnClick()
{
	MdzString strName = MdzStdDialog::ShowInput("Name", "Add Layout");
	if (strName.GetLength())
		AddNode(LayoutNode::Create(strName));

}

void TabPanelNode::addOnSize_OnClick()
{
	AddCallbackNode("OnSize", "int width, int height", "void");

}







void MaFenetre::propPanel_OnSize(int width, int height)
{
	if (pSelNode)
		pSelNode->Activate(&propPanel);

}

void MaFenetre::OnSize(MdzRect &rc)
{
	rc.Inflate(-5, -5);

	MdzRect rc2 = rc;

	rc.bottom -= TABHEIGHT+5;
	divPanel.SetBounds(MdzBounds(rc));

	rc2.top = rc2.bottom - TABHEIGHT;
	tabCtrl.SetBounds(MdzBounds(rc2));

}

MdzString CtrlNode::GetLabelText()
{
	return MdzString();
}

MdzString CtrlNode::GetLabelWidth()
{
	return MdzString();
}

MdzString LabelledCtrlNode::GetLabelText()
{
	return form.GetValue("LabelText");
}

void LabelledCtrlNode::InitProps()
{
	CtrlNode::InitProps();

	form.SetValue("Height", "20");

	if (GetParentClassName().Equals("LayoutNode"))
	{
		form.AddProp(Prop::TEXT, "LabelText", "");
		form.AddProp(Prop::DIGIT, "LabelWidth", "100");
	}
}

MdzString LabelledCtrlNode::GetLabelWidth()
{
	return form.GetValue("LabelWidth");

}

VarNode::VarNode()
{
	size = 0;
}

void Form::SetValue(MdzString name, MdzString value)
{
	for(int i = 0; i < (int)props.GetCount(); i++)
	{
		if (props[i].GetName().Equals(name))
		{
			props[i].SetValue(value);
			break;
		}
	}
}

void MethodNode::code_OnEnter()
{
	int startIdx = code.GetStartIndex();
	int idx = SendMessage(code.GetHandle(), EM_LINEFROMCHAR , startIdx, 0);
	if (idx > 0)
	{
		MdzString strLine = code.GetLineAt(idx-1);
		int nbTab = 0;
		int i = 0;
		while (strLine.GetCharAt(i++) == '\t')
			nbTab++;
		for (i = 0; i < nbTab; i++)
			code.ReplaceSelection("\t");			


	}
}

void MethodNode::code_OnLButtonDblClick(int x, int y)
{
	int idx = code.GetIndexFromPos(x, y);
	MdzString strCode = code.GetText();
	int cur = idx;
	while (cur >= 0 && IsGoodChar(strCode.GetCharAt(cur)))
		cur--;

	int startIdx = cur;
	cur = idx;
	while (cur < strCode.GetLength() && IsGoodChar(strCode.GetCharAt(cur)))
		cur++;

	code.SetSelection(startIdx+1, cur);

}

BOOL MethodNode::IsGoodChar(UCHAR ch)
{
	if (IsCharAlphaNumeric(ch))
		return TRUE;

	if (ch == '_')
		return TRUE;

	return FALSE;
}

BOOL MethodNode::IsInQuote(int iIndex)
{
	MdzString strCode = code.GetText();

	int nbQuote = 0;
	for(int i = 0; i < iIndex; i++)
	{
		CHAR ch = strCode.GetCharAt(i);
		if (ch == '\\')
		{
			i++;
			continue;
		}
		if (ch == '"')
			nbQuote++;
	}

	return ((nbQuote % 2) == 1);
}

void MaFenetre::leftPanel_OnSize(int width, int height)
{
#define TOOLBAR_HEIGHT 25
	treeCtrl.SetBounds(0, 0, width, height - TOOLBAR_HEIGHT);
	toolBar2.SetBounds(0, height-TOOLBAR_HEIGHT, width, TOOLBAR_HEIGHT);
}

int PanelNode::OnCompareChild(MdzTreeNode *pNode1, MdzTreeNode *pNode2)
{
	MdzString strClassNode1 = pNode1->GetClass();
	MdzString strClassNode2 = pNode2->GetClass();

	if (!strClassNode1.Equals(strClassNode2))
	{
		if (strClassNode1.Equals("CallbackNode"))
			return-1;
		
		if (strClassNode2.Equals("CallbackNode"))
			return 1;

	}

	return MyNode::OnCompareChild(pNode1, pNode2);
}

void TreeCtrlNode::InitProps()
{
	CtrlNode::InitProps();
	AddMenuItem("Add OnSelChange", MDZ_CBK1(TreeCtrlNode, addOnSelChange_OnClick));
	AddMenuItem("Add OnRightClick", MDZ_CBK1(TreeCtrlNode, addOnRightClick_OnClick));
}

void TreeCtrlNode::addOnSelChange_OnClick()
{
	AddCallbackNode("OnSelChange", "", "void");

}

void TreeCtrlNode::addOnRightClick_OnClick()
{
	AddCallbackNode("OnRightClick", "int x, int y", "void");

}

MenuNode::MenuNode()
{
	type = "MdzMenu";

}

MenuItemNode::MenuItemNode()
{
	type = "MdzMenuItem";

}

void ClassNode::GetVarList(MdzString strPrefix, MdzStrVector &vector)
{
	MyNode* pNode = (MyNode*)GetNext();

	while (1)
	{
		if (!pNode)
			break;
		
		if (pNode->GetPosition() == GetPosition())
			break;

		if (pNode->IsInstanceOf("VarNode"))
		{
			if (strPrefix.GetLength() == 0)
				vector.Add(pNode->GetRealName());
			else if (pNode->GetRealName().GetIndexOf(strPrefix) == 0)
				vector.Add(pNode->GetRealName());


		}

		pNode = (MyNode*)pNode->GetNext();
	}
}

void MethodNode::DisplayVarList()
{
	if (!code.HasFocus())
		return;

	ClassNode* pClassNode = ((MaFenetre*)MdzFrame::GetMainFrame())->GetClassNode(GetFatherClass());
	if (!pClassNode)
		return;

	int startIdx = code.GetStartIndex();
	if (startIdx == 0)
		return;
	MdzString strPrefix = GetKeyWord(code.GetText(), startIdx-1);
	MdzStrVector vector;
	pClassNode->GetVarList(strPrefix, vector);
	if (vector.GetCount() == 0)
		return;
	if (vector.GetCount() == 1)
	{
		code.ReplaceSelection(vector[0].GetSubString(strPrefix.GetLength()));
		return;
	}

	MdzMenu menu;

	for(int i = 0; i < (int)vector.GetCount(); i++)
	{
		menu.AddItem(vector[i]);
	}
	MdzPoint pt;
	if (!code.GetPosFromIndex(startIdx-1, pt))
		return;
	MdzMenuItem* pItem = menu.Track(code, pt.x, pt.y + 15);
	if (pItem)
	{
		code.ReplaceSelection(pItem->GetCaption().GetSubString(strPrefix.GetLength()));
	}

}

void MaFenetre::OnDisplayVarList()
{
	MyNode* pNode = (MyNode*)treeCtrl.GetSelNode();
	if (pNode && pNode->IsInstanceOf("MethodNode"))
	{
		((MethodNode*)pNode)->DisplayVarList();
	}
}

MdzString ClassNode::GetBaseClass()
{
	return baseClass;
}

ShortCutCtrl::ShortCutCtrl()
{
	Add(cbChanger);
	Add(cbChar);
	cbChanger.AddItem("(None)");
	cbChanger.AddItem("Ctrl", (LPVOID)MdzKey::K_CTRL);
	cbChanger.AddItem("Shift", (LPVOID) MdzKey::K_SHIFT);
	cbChanger.AddItem("Alt", (LPVOID) MdzKey::K_ALT);
	cbChanger.SetSelIndex(0);
	cbChanger.SetOnSelChange(MDZ_CBK1(ShortCutCtrl, cbChanger_OnSelChange));
	cbChar.AddItem("(None)");

	InitChar(FALSE);
	cbChar.SetSelIndex(0);

}

void ShortCutCtrl::OnSize(int width, int height)
{
	cbChanger.SetBounds(0, 0, width / 2, height);
	cbChar.SetBounds(width/2, 0, width / 2, height);

}

ShortCutCtrl* Prop::AddShortCut(MdzLayout &layout, MdzString label, MdzString values)
{
	ShortCutCtrl* pCtrl = new ShortCutCtrl;
	AddLabelledCtrl(layout, label, *pCtrl);
	return pCtrl;
}

void ShortCutCtrl::cbChanger_OnSelChange()
{
	MdzString strChar = cbChar.GetSelItem();
	InitChar(!cbChanger.GetSelItem().Equals("(None)"));

	if (!cbChar.SetSelItem(strChar))
		cbChar.SetSelIndex(0);

}

DWORD ShortCutCtrl::GetValue()
{
	return MAKELONG(cbChar.GetDataAt(cbChar.GetSelIndex()),
		cbChanger.GetDataAt(cbChanger.GetSelIndex()));
}

void ShortCutCtrl::SetValue(DWORD iValue)
{
	int iChangerValue = HIWORD(iValue);
	for(UINT i = 0; i < cbChanger.GetItemCount(); i++)
	{
		if (cbChanger.GetDataAt(i) == (LPVOID)iChangerValue)
		{
			cbChanger.SetSelIndex(i);
			break;
		}
	
	}

	InitChar(iChangerValue != 0);

	int iCharValue = LOWORD(iValue);
	for(i = 0; i < cbChar.GetItemCount(); i++)
	{
		if (cbChar.GetDataAt(i) == (LPVOID)iCharValue)
		{
			cbChar.SetSelIndex(i);
			break;
		}
	
	}
}

void ShortCutCtrl::InitChar(BOOL bFull)
{
	cbChar.RemoveAllItems();
	cbChar.AddItem("(None)");
	for(int i = 0; i < 12; i++)
		cbChar.AddItem(MdzString::Format("F%d", i+1), LPVOID(0x70+i));
	if (!bFull)
		return;

	for(char ch = 'A'; ch <= 'Z'; ch++)
		cbChar.AddItem(MdzString::Format("%c", ch), (LPVOID)ch);

}
