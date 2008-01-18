#							-*- shell-script -*-

AT_SETUP(Bambara)
dnl      -------

cat <<'EOF' >example
N ye n ka nsiirin nin da Amadu Tara la.  Jamanatigi d=F2 tun b=E8 yen muso
k=F2n=F2nt=F2 de tun b=E8 a f=E8.  Muso k=F2n=F2nt=F2 b=E8=E8 ye denk=E8 ke=
len kelen
wolo a ye.  A denk=E8 kelen kelen b=E8=E8 fana t=F2g=F2 ye ko Amadu.  Amadu
ninw, u ba t=F2g=F2 de tun b=E8 da u kan ka u b=F2 ny=F2g=F2n na.  Amadu b=
=E8=E8
ncinin tun ye denk=E8 ninw b=E8=E8 la d=F2g=F2nin o de tun ye Amadu Tara ye.
O tun ye jamanatigi nin ka taramuso den ye.
EOF

cat <<'EOF' >transcript
TiB5ZSBuIGthIG5zaWlyaW4gbmluIGRhIEFtYWR1IFRhcmEgbGEuICBKYW1hbmF0aWdpIGRvYCB0
dW4gYmVgIHllbiBtdXNvCmtvYG5vYG50b2AgZGUgdHVuIGJlYCBhIGZlYC4gIE11c28ga29gbm9g
bnRvYCBiZWBlYCB5ZSBkZW5rZWAga2VsZW4ga2VsZW4Kd29sbyBhIHllLiAgQSBkZW5rZWAga2Vs
ZW4ga2VsZW4gYmVgZWAgZmFuYSB0b2Bnb2AgeWUga28gQW1hZHUuICBBbWFkdQpuaW53LCB1IGJh
IHRvYGdvYCBkZSB0dW4gYmVgIGRhIHUga2FuIGthIHUgYm9gIG55b2Bnb2BuIG5hLiAgQW1hZHUg
YmVgZWAKbmNpbmluIHR1biB5ZSBkZW5rZWAgbmludyBiZWBlYCBsYSBkb2Bnb2BuaW4gbyBkZSB0
dW4geWUgQW1hZHUgVGFyYSB5ZS4KTyB0dW4geWUgamFtYW5hdGlnaSBuaW4ga2EgdGFyYW11c28g
ZGVuIHllLgo=
EOF

AT_CHECK(
[cp transcript expout
recode ../,bambara/qp..t-bambara/64 < example
], 0, expout)

AT_CHECK(
[cp example expout
recode ../,t-bambara/64..bambara/qp < transcript
], 0, expout)

AT_CLEANUP(expout example transcript)

AT_SETUP(Peul Fulfulde)
dnl      -------------

cat <<'EOF' >example
Woni =B2on haa =B1ooyi.  Oya mar=B2o jiwo kaanaro on maayi.  Suka no
ton no wi=BDee Buubu Yee=B1e, on suka e nder sukaa=B1e aduna on alaa
bur=B2o mo labeede.  On suka yahi safaari, o woni ton haa nee=B1i ---
wa duu=B1i sappo o ko fawi! --- Yumma makko maraa =B1i=B2=B2o
go=BDo si wonaa kanko.
EOF

cat <<'EOF' >transcript
V29uaSBkW29uIGhhYSBiW29veWkuICBPeWEgbWFyZFtvIGppd28ga2FhbmFybyBvbiBtYWF5aS4g
IFN1a2Egbm8KdG9uIG5vIHdpP15lZSBCdXVidSBZZWViW2UsIG9uIHN1a2EgZSBuZGVyIHN1a2Fh
YltlIGFkdW5hIG9uIGFsYWEKYnVyZFtvIG1vIGxhYmVlZGUuICBPbiBzdWthIHlhaGkgc2FmYWFy
aSwgbyB3b25pIHRvbiBoYWEgbmVlYltpIC0tLQp3YSBkdXViW2kgc2FwcG8gbyBrbyBmYXdpISAt
LS0gWXVtbWEgbWFra28gbWFyYWEgYltpZFtkW28KZ28/Xm8gc2kgd29uYWEga2Fua28uCg==
EOF

AT_CHECK(
[cp transcript expout
recode ../,fulfulde/qp..t-fulfulde/64 < example
], 0, expout)

AT_CHECK(
[cp example expout
recode ../,t-fulfulde/64..fulfulde/qp < transcript
], 0, expout)

AT_CLEANUP(expout example transcript)

AT_SETUP(Lingala)
dnl      -------

cat <<'EOF' >example
Mob=E1li m=B6k=B6, azal=E1k=ED s=E9 y=E9m=B6k=B6 n=E1 bil=B6k=F8 m=EDngi.
Mok=F8l=F8 m=B6k=B6 alob=ED: t=EDk=E1 n=E1k=F0nd=F0 koluka mw=E3s=ED.  Ak=
=F0=ED.
At=E1mb=F3l=ED k=E1 k=E1 k=E1, ak=F3m=ED namb=F3ka m=B6k=B6, ak=FAt=ED w=E2=
n=E1 Ntaba.
Ntaba at=FAn=ED y=FD: ndeko, =F5kok=F0nd=F0 w=E1pi?  Moto alob=ED: n=E3kok=
=F0nd=F0
koluka mw=E3si yakob=E1la nay=E9.  Ntaba alob=ED: zil=E1 ng=E1=ED t=F3k=F0n=
d=F0
el=F8ng=B6.
EOF

cat <<'EOF' >transcript
TW9iYVwnbGkgbW9bJ2tvWycsIGF6YWxhXCdraVwnIHNlXCcgeWVcJ21vWydrb1snIG5hXCcgYmls
b1sna29bIG1pXCduZ2kuCk1va29bbG9bIG1vWydrb1snIGFsb2JpXCc6IHRpXCdrYVwnIG5hXCdr
ZVtuZGVbIGtvbHVrYSBtd2FcdnNpXCcuICBBa2VbaVwnLgpBdGFcJ21ib1wnbGlcJyBrYVwnIGth
XCcga2FcJywgYWtvXCdtaVwnIG5hbWJvXCdrYSBtb1sna29bJywgYWt1XCd0aVwnIHdhXm5hXCcg
TnRhYmEuCk50YWJhIGF0dVwnbmlcJyB5ZVx2OiBuZGVrbywgb1x2a29rZVtuZGVbIHdhXCdwaT8g
IE1vdG8gYWxvYmlcJzogbmFcdmtva2VbbmRlWwprb2x1a2EgbXdhXHZzaSB5YWtvYmFcJ2xhIG5h
eWVcJy4gIE50YWJhIGFsb2JpXCc6IHppbGFcJyBuZ2FcJ2lcJyB0b1wna2VbbmRlWwplbG9bbmdv
WycuCg==
EOF

AT_CHECK(
[cp transcript expout
recode ../,lingala/qp..t-lingala/64 < example
], 0, expout)

AT_CHECK(
[cp example expout
recode ../,t-lingala/64..lingala/qp < transcript
], 0, expout)

AT_CLEANUP(expout example transcript)

AT_SETUP(Sango)
dnl      -----

cat <<'EOF' >example
M=E2ley=F6mb=F6 ayeke mb=EAn=EE wakua t=EE let=E4a.  Lo yeke zo s=F4 lo y=
=EA kua t=EE
lo m=EEngi.  Azo t=EE y=E2 t=EE vaka s=F4 s=EF lo yeke l=E4ng=F6 da=E4 s=F4=
 ay=EA
t=EBn=EB t=EE lo m=EEngi ngbanga t=EE nz=F6n=EE dut=EF t=EE lo na y=E2ng=E2=
da t=EE
lo na =E2s=EBw=E4 t=EE lo.
EOF

cat <<'EOF' >transcript
TWFebGV5byJtYm8iIGF5ZWtlIG1iZV5uaV4gd2FrdWEgdGleIGxldGEiYS4gIExvIHlla2Ugem8g
c29eIGxvIHllXiBrdWEgdGleCmxvIG1pXm5naS4gIEF6byB0aV4geWFeIHRpXiB2YWthIHNvXiBz
aSIgbG8geWVrZSBsYSJuZ28iIGRhYSIgc29eIGF5ZV4KdGUibmUiIHRpXiBsbyBtaV5uZ2kgbmdi
YW5nYSB0aV4gbnpvIm5pXiBkdXRpIiB0aV4gbG8gbmEgeWFebmdhXmRhIHRpXgpsbyBuYSBhXnNl
IndhIiB0aV4gbG8uCg==
EOF

AT_CHECK(
[cp transcript expout
recode ../,sango/qp..t-sango/64 < example
], 0, expout)

AT_CHECK(
[cp example expout
recode ../,t-sango/64..sango/qp < transcript
], 0, expout)

AT_CLEANUP(expout example transcript)

AT_SETUP(Wolof)
dnl      -----

cat <<'EOF' >example
Ca 1965 ba leegi, Suwaahili mooy l=E0kk r=E9=E9wum Tansani.  L=E0kk woowu
la=BAuy j=E0ngalee ci daara yu ndaw yi ag yu yem yi.  Ci moom la =BAuy
jottalee xabaar yi =BAuy wax ag yi =BAuy bind.
EOF

cat <<'EOF' >transcript
Q2EgMTk2NSBiYSBsZWVnaSwgU3V3YWFoaWxpIG1vb3kgbGFga2sgcmVcJ2VcJ3d1bSBUYW5zYW5p
LiAgTGFga2sgd29vd3UKbGFuXnV5IGphYG5nYWxlZSBjaSBkYWFyYSB5dSBuZGF3IHlpIGFnIHl1
IHllbSB5aS4gIENpIG1vb20gbGEgbl51eQpqb3R0YWxlZSB4YWJhYXIgeWkgbl51eSB3YXggYWcg
eWkgbl51eSBiaW5kLgo=
EOF

AT_CHECK(
[cp transcript expout
recode ../,wolof/qp..t-wolof/64 < example
], 0, expout)

AT_CHECK(
[cp example expout
recode ../,t-wolof/64..wolof/qp < transcript
], 0, expout)

AT_CLEANUP(expout example transcript)
