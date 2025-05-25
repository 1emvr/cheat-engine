bits 32

;------------------;
;ULONG getESP(void);
;------------------;
global getESP
getESP:
mov eax,esp
add eax,4 ;for the push of the call
ret

;-----------------;
;inportb(int port); returns a byte from the given port
;-----------------;
global inportb
inportb:
push ebp
mov ebp,esp

mov edx,[ebp+8]
in al,dx

pop ebp
ret ;4 ; (no params, in cdecl frees the caller)

;----------------------------------------------------------;
;void cpuid(ULONG *eax, ULONG *ebx, ULONG *ecx, ULONG *edx);
;----------------------------------------------------------;
global _cpuid
_cpuid:
push ebp
mov ebp,esp ;ebp=free to use, ebp+4 is address of caller, +8 is param1

push eax
push ebx
push ecx
push edx

; obtain the 4 arguments from the stack at 8+ and derefernce them (cpuid(&a, &b, &c, &d)).
; CPUID returns processor identification and feature information in the EAX, EBX, ECX, and EDX registers. 
; The instruction’s output is dependent on the contents of the EAX register upon execution (in some cases, ECX as well).
mov eax,[ebp+8]
mov eax,[eax]
mov ebx,[ebp+12]
mov ebx,[ebx]
mov ecx,[ebp+16]
mov ecx,[ecx]
mov edx,[ebp+20]
mov edx,[edx]
cpuid

push ebx
mov ebx,eax
mov eax,[ebp+8]  ;eax
mov [eax],ebx
pop ebx

mov eax,[ebp+12]  ;ebx
mov [eax],ebx

mov eax,[ebp+16]  ;ecx
mov [eax],ecx

mov eax,[ebp+20]  ;edx
mov [eax],edx

pop edx
pop ecx
pop ebx
pop eax

mov esp,ebp
pop ebp
ret ;16

;---------------------------;
;void spinlock(int *lockvar); // spin this thread forever until the lock is released
;---------------------------;
global spinlock
spinlock:
push ebp
mov ebp,esp

push ebx
mov ebx,[ebp+8] ;ebx now contains the address of the lock (spinlock(&a)))

spinlock_loop:
;serialize
pushad
xor eax,eax
cpuid ;serialize
popad ;pop stack values to all the GP registers

;check lock
cmp dword [ebx],0
je spinlock_getlock
pause
jmp spinlock_loop

spinlock_getlock:
mov eax,1
xchg eax,[ebx] ;try to lock
cmp eax,0 ;test if successful
jne spinlock_loop

pop ebx
pop ebp
ret ;4


;-----------------------------------------;
;void outportb(short int port, char value); // I/O port writer
;-----------------------------------------;
global outportb
outportb:
push ebp
mov ebp,esp
mov edx,[ebp+8]  ;port
mov eax,[ebp+12] ;value

out dx,al

pop ebp
ret ;8

;--------------------;
;void showstate(void);
;--------------------;
global showstate
extern showstatec
showstate:

pushad      ;+4
pushfd      ;+32
mov eax,cr0
push eax    ;+4 
;mov eax,cr1
mov eax,0
push eax    ;+4 
mov eax,cr2
push eax    ;+4 
mov eax,cr3
push eax    ;+4 

sub esp,8
sgdt [esp]
mov eax,[esp+2]
add esp,8
push eax    ;+4 

sub esp,8
sidt [esp]
mov eax,[esp+2]
add esp,8
push eax    ;+4 

push esp    ;+4
call showstatec
add esp,4+32+4+4+4+4+4+4+4
ret
