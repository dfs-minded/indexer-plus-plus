!include LogicLib.nsh
!include WinMessages.nsh
 
!macro ShellExecWait verb app param workdir show exitoutvar ;only app and show must be != "", every thing else is optional
#define SEE_MASK_NOCLOSEPROCESS 0x40 
System::Store S
System::Call '*(&i60)i.r0'
System::Call '*$0(i 60,i 0x40,i $hwndparent,t "${verb}",t $\'${app}$\',t $\'${param}$\',t "${workdir}",i ${show})i.r0'
System::Call 'shell32::ShellExecuteEx(ir0)i.r1 ?e'
${If} $1 <> 0
	System::Call '*$0(is,i,i,i,i,i,i,i,i,i,i,i,i,i,i.r1)' ;stack value not really used, just a fancy pop ;)
	System::Call 'kernel32::WaitForSingleObject(ir1,i-1)'
	System::Call 'kernel32::GetExitCodeProcess(ir1,*i.s)'
	System::Call 'kernel32::CloseHandle(ir1)'
${EndIf}
System::Free $0
!if "${exitoutvar}" == ""
	pop $0
!endif
System::Store L
!if "${exitoutvar}" != ""
	pop ${exitoutvar}
!endif
!macroend