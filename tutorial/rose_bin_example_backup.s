/*
   ROSE NOT CONFIGURED FOR BINARY ANALYSIS

  Example Output (rose_bin_example_backup.s)
*/
        
/* Block 0x0 */ 
/* Function 0x8048278: _init */ 
 8048278:	push   ebp
 8048279:	mov    ebp,esp
 804827b:	sub    esp,0x8
 804827e:	call   80482e4 <call_gmon_start>
 8048283:	call   8048340 <frame_dummy>
 8048288:	call   8048460 <__do_global_ctors_aux>
 804828d:	leave  
 804828e:	ret    
/* Function 0x8048290: malloc@plt */ 
 8048290:	push   DWORD PTR [0x80495a0]
 8048296:	jmp    DWORD PTR [0x80495a4]
 804829c:	add    DWORD PTR [eax],al
 804829e:	add    DWORD PTR [eax],al
 80482a0:	jmp    DWORD PTR [0x80495a8]
 80482a6:	push   0x0
 80482ab:	jmp    0x8048290 <_init+0x18>
 80482b0:	jmp    DWORD PTR [0x80495ac]
 80482b6:	push   0x8
 80482bb:	jmp    0x8048290 <_init+0x18>
 80482c0:	xor    ebp,ebp
 80482c2:	pop    esi
 80482c3:	mov    ecx,esp
 80482c5:	and    esp,0xfffffff0
 80482c8:	push   eax
 80482c9:	push   esp
 80482ca:	push   edx
 80482cb:	push   0x8048414
 80482d0:	push   0x80483c0
 80482d5:	push   ecx
 80482d6:	push   esi
 80482d7:	push   0x8048364
 80482dc:	call   80482b0 <__libc_start_main@plt>
 80482e1:	hlt    
 80482e2:	nop    
 80482e3:	nop    
/* Function 0x80482e4: call_gmon_start+0x1d */ 
 80482e4:	push   ebp
 80482e5:	mov    ebp,esp
 80482e7:	push   ebx
 80482e8:	call   80482ed <call_gmon_start+0x9>
 80482ed:	pop    ebx
 80482ee:	add    ebx,0x12af
 80482f4:	push   edx
 80482f5:	mov    eax,DWORD PTR [0xfffffffc+ebx]
 80482fb:	test   eax,eax
 80482fd:	je     0x8048301 <call_gmon_start+0x1d>
 80482ff:	call   eax
 8048301:	pop    eax
 8048302:	pop    ebx
 8048303:	leave  
 8048304:	ret    
 8048305:	nop    
 8048306:	nop    
 8048307:	nop    
 8048308:	nop    
 8048309:	nop    
 804830a:	nop    
 804830b:	nop    
 804830c:	nop    
 804830d:	nop    
 804830e:	nop    
 804830f:	nop    
/* Function 0x8048310: __do_global_dtors_aux+0x1b */ 
 8048310:	push   ebp
 8048311:	mov    ebp,esp
 8048313:	sub    esp,0x8
 8048316:	cmp    DWORD PTR [0x80495bc],0x0
 804831d:	je     0x804832b <__do_global_dtors_aux+0x1b>
 804831f:	jmp    0x804833d <__do_global_dtors_aux+0x2d>
 8048321:	add    eax,0x4
 8048324:	mov    DWORD PTR [0x80495b8],eax
 8048329:	call   edx
 804832b:	mov    eax,DWORD PTR [0x80495b8]
 8048330:	mov    edx,DWORD PTR [eax]
 8048332:	test   edx,edx
 8048334:	jne    0x8048321 <__do_global_dtors_aux+0x11>
 8048336:	mov    DWORD PTR [0x80495bc],0x1
 804833d:	leave  
 804833e:	ret    
 804833f:	nop    
/* Function 0x8048340: frame_dummy */ 
 8048340:	push   ebp
 8048341:	mov    ebp,esp
 8048343:	sub    esp,0x8
 8048346:	mov    eax,DWORD PTR [0x80494cc]
 804834b:	test   eax,eax
 804834d:	je     0x8048361 <frame_dummy+0x21>
 804834f:	mov    eax,0x0
 8048354:	test   eax,eax
 8048356:	je     0x8048361 <frame_dummy+0x21>
 8048358:	mov    DWORD PTR [esp],0x80494cc
 804835f:	call   eax
 8048361:	leave  
 8048362:	ret    
 8048363:	nop    
/* Function 0x8048364: main+0x42 */ 
 8048364:	lea    ecx,[0x4+esp]
 8048368:	and    esp,0xfffffff0
 804836b:	push   DWORD PTR [0xfffffffc+ecx]
 804836e:	push   ebp
 804836f:	mov    ebp,esp
 8048371:	push   ecx
 8048372:	sub    esp,0x14
 8048375:	mov    DWORD PTR [esp],0x28
 804837c:	call   80482a0 <malloc@plt>
 8048381:	mov    DWORD PTR [0xfffffff0+ebp],eax
 8048384:	mov    DWORD PTR [0xfffffff4+ebp],0x0
 804838b:	mov    DWORD PTR [0xfffffff4+ebp],0x0
 8048392:	jmp    0x80483a6 <main+0x42>
 8048394:	mov    eax,DWORD PTR [0xfffffff4+ebp]
 8048397:	shl    eax,0x2
 804839a:	add    eax,DWORD PTR [0xfffffff0+ebp]
 804839d:	mov    DWORD PTR [eax],0x5
 80483a3:	inc    DWORD PTR [0xfffffff4+ebp]
 80483a6:	cmp    DWORD PTR [0xfffffff4+ebp],0x9
 80483aa:	jle    0x8048394 <main+0x30>
 80483ac:	mov    eax,DWORD PTR [0xfffffff0+ebp]
 80483af:	add    eax,0x30
 80483b2:	mov    eax,DWORD PTR [eax]
 80483b4:	mov    DWORD PTR [0xfffffff8+ebp],eax
 80483b7:	add    esp,0x14
 80483ba:	pop    ecx
 80483bb:	pop    ebp
 80483bc:	lea    esp,[0xfffffffc+ecx]
 80483bf:	ret    
/* Function 0x80483c0: __libc_csu_init+0xe */ 
 80483c0:	push   ebp
 80483c1:	mov    ebp,esp
 80483c3:	push   edi
 80483c4:	push   esi
 80483c5:	push   ebx
 80483c6:	sub    esp,0xc
 80483c9:	call   80483ce <__libc_csu_init+0xe>
 80483ce:	pop    ebx
 80483cf:	add    ebx,0x11ce
 80483d5:	call   8048278 <_init>
 80483da:	lea    eax,[0xffffff20+ebx]
 80483e0:	lea    edx,[0xffffff20+ebx]
 80483e6:	mov    DWORD PTR [0xfffffff0+ebp],eax
 80483e9:	sub    eax,edx
 80483eb:	xor    esi,esi
 80483ed:	sar    eax,0x2
 80483f0:	cmp    esi,eax
 80483f2:	jae    0x804840a <__libc_csu_init+0x4a>
 80483f4:	mov    edi,edx
 80483f6:	mov    esi,esi
 80483f8:	call    PTR [edx+esi*4]
 80483fb:	mov    ecx,DWORD PTR [0xfffffff0+ebp]
 80483fe:	sub    ecx,edi
 8048400:	inc    esi
 8048401:	sar    ecx,0x2
 8048404:	cmp    esi,ecx
 8048406:	mov    edx,edi
 8048408:	jb     0xf8
 804840a:	add    esp,0xc
 804840d:	pop    ebx
 804840e:	pop    esi
 804840f:	pop    edi
 8048410:	leave  
 8048411:	ret    
 8048412:	mov    esi,esi
/* Function 0x8048414: __libc_csu_fini+0xb */ 
 8048414:	push   ebp
 8048415:	mov    ebp,esp
 8048417:	push   edi
 8048418:	push   esi
 8048419:	push   ebx
 804841a:	call   804841f <__libc_csu_fini+0xb>
 804841f:	pop    ebx
 8048420:	add    ebx,0x117d
 8048426:	lea    eax,[0xffffff20+ebx]
 804842c:	lea    edi,[0xffffff20+ebx]
 8048432:	sub    eax,edi
 8048434:	sar    eax,0x2
 8048437:	sub    esp,0xc
 804843a:	lea    esi,[0xffffffff+eax]
 804843d:	jmp    0x8048444 <__libc_csu_fini+0x30>
 804843f:	nop    
 8048440:	call    PTR [edi+esi*4]
 8048443:	dec    esi
 8048444:	cmp    esi,0xffffffff
 8048447:	jne    0x8048440 <__libc_csu_fini+0x2c>
 8048449:	call   8048494 <_fini>
 804844e:	add    esp,0xc
 8048451:	pop    ebx
 8048452:	pop    esi
 8048453:	pop    edi
 8048454:	leave  
 8048455:	ret    
 8048456:	nop    
 8048457:	nop    
 8048458:	nop    
 8048459:	nop    
 804845a:	nop    
 804845b:	nop    
 804845c:	nop    
 804845d:	nop    
 804845e:	nop    
 804845f:	nop    
/* Function 0x8048460: __do_global_ctors_aux */ 
 8048460:	push   ebp
 8048461:	mov    ebp,esp
 8048463:	push   ebx
 8048464:	mov    ebx,0x80494bc
 8048469:	sub    esp,0x4
 804846c:	mov    eax,DWORD PTR [0x80494bc]
 8048471:	cmp    eax,0xffffffff
 8048474:	je     0x804848c <__do_global_ctors_aux+0x2c>
 8048476:	lea    esi,[esi]
 8048479:	lea    edi,[edi]
 8048480:	sub    ebx,0x4
 8048483:	call   eax
 8048485:	mov    eax,DWORD PTR [ebx]
 8048487:	cmp    eax,0xffffffff
 804848a:	jne    0x8048480 <__do_global_ctors_aux+0x20>
 804848c:	pop    eax
 804848d:	pop    ebx
 804848e:	pop    ebp
 804848f:	nop    
 8048490:	ret    
 8048491:	nop    
 8048492:	nop    
 8048493:	nop    
/* Function 0x8048494: _fini */ 
 8048494:	push   ebp
 8048495:	mov    ebp,esp
 8048497:	push   ebx
 8048498:	call   804849d <_fini+0x9>
 804849d:	pop    ebx
 804849e:	add    ebx,0x10ff
 80484a4:	push   eax
 80484a5:	call   8048310 <__do_global_dtors_aux>
 80484aa:	pop    ecx
 80484ab:	pop    ebx
 80484ac:	leave  
 80484ad:	ret    

