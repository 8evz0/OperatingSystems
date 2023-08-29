section .data
	vbox_message db "VirtualBox hypervisor detected", 0
	vbox_message_len equ $ - vbox_message
	vmware_message db "VMWare hypervisor detected", 0
	vmware_message_len equ $ - vmware_message
	kvm_message db "KVM hypervisor detected", 0
	kvm_message_len equ $ - kvm_message
	kvm_error_message db "Error: Failed to exit KVM", 0
	kvm_error_message_len equ $ - kvm_error_message  
	kvm_exit_code equ 0x2BADB002 ; Код выхода из KVM

section .text
	global _start

_start:
	; ? VirtualBox
	mov eax, 0x564D5868 ; "VMXh" сигнатура
	mov ebx, 0 
	mov ecx, 10
	mov edx, 0x00000000
	cpuid ; Вызов функции CPUID
	cmp ebx, 0x564D5868 
	jne check_vmware 
	; + VirtualBox
	mov eax, 4 
	mov ebx, 1
	mov ecx, vbox_message
	mov edx, vbox_message_len 
	int 0x80
	jmp exit
check_vmware:
	; ? VMWare
	mov eax, 0x564D5868 ; "VMXh" сигнатура
	mov ebx, 0x00000000
	mov ecx, 10 ; Версия
	mov edx, 0x4D566572 ; "VMeR" сигнатура
	cpuid
	cmp edx, 0x4D566572 
	jne check_kvm

	; + VMWare
	mov eax, 4 
	mov ebx, 1 
	mov ecx, vmware_message 
	mov edx, vmware_message_len 
	int 0x80 
	jmp exit

check_kvm:
	mov eax, 0x40000000 ; "KVMK" сигнатура
	mov ebx, 0 
	mov ecx, 0 
	mov edx, 0x4B4D564B ; "KVMK" сигнатура
	cpuid
	cmp ebx, 0x4B4D564B 
	jne exit 

	; + KVM
	mov eax, 4
	mov ebx, 1 ; Дескриптор файла - stdout
	mov ecx, kvm_message
	mov edx, kvm_message_len
	int 0x80 

   mov eax, 0x12345678 
   mov ebx, 0 
   xor ecx, ecx
   xor edx, edx
   xor esi, esi 
   xor edi, edi 
   xor ebp, ebp 
   xor eax, eax 
   db 0x0F, 0x01, 0xD9
	
	mov eax, 4
	mov ebx, 1 
	mov ecx, kvm_error_message
	mov edx, kvm_error_message_len
	int 0x80 
exit:
	mov eax, 1 ; 
	xor ebx, ebx
	int 0x80

