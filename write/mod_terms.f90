module terms
use constant
implicit none

contains

subroutine calc_terms(iparton,r,bigR,ej,alphas,resum)
implicit none
real(8) :: C, b0
real(8), external :: ddilog
integer :: iparton
real(8) :: nxr, r, bigR, ej
real(8) :: resum
real(8) :: alphas
complex(8) :: initQuark, initGluon

cosr=Cos(r)
cosbigR=Cos(bigR)
cosro2=Cos(r/2d0)
cosbigRo2=Cos(bigR/2d0)
sinr=Sin(r)
sinbigR=Sin(bigR)
secr=1/Cos(r)
secbigR=1/Cos(bigR)
cscr=1/Sin(r)
cscbigR=1/Sin(bigR)
cotr=1/Tan(r)
cotbigR=1/Tan(bigR)
sinro2=Sin(r/2d0)
sinbigRo2=Sin(bigR/2d0)
tanro2=Tan(r/2d0)
tanbigRo2=Tan(bigR/2d0)
cotro2=1/Tan(r/2d0)
cotbigRo2=1/Tan(bigR/2d0)

sin2r=Sin(2*r)
cos2r=Cos(2*r)
sin3r=Sin(3*r)
cos3r=Cos(3*r)
sin2bigR=Sin(2*bigR)
cos2bigR=Cos(2*bigR)
sin3bigR=Sin(3*bigR)
cos3bigR=Cos(3*bigR)

nxr=r
r2=nxr**2
bigR2=bigR**2
r4=nxr**4
bigR4=bigR**4
r6=nxr**6
bigR6=bigR**6
r8=nxr**8
bigR8=bigR**8
r10=nxr**10
bigR10=bigR**10
r12=nxr**12
bigR12=bigR**12
r14=nxr**14
bigR14=bigR**14
r16=nxr**16
bigR16=bigR**16
r18=nxr**18
bigR18=bigR**18
r20=nxr**20
bigR20=bigR**20
r22=nxr**22
bigR22=bigR**22
r24=nxr**24
bigR24=bigR**24
r26=nxr**26
bigR26=bigR**26
r28=nxr**28
bigR28=bigR**28
r30=nxr**30
bigR30=bigR**30
r32=nxr**32
bigR32=bigR**32
r34=nxr**34
bigR34=bigR**34
r36=nxr**36
bigR36=bigR**36

logr=log(nxr)
logbigR=log(bigR)
cosrP2=cosr**2
cosrP3=cosr**3
cosrP4=cosr**4
cosrP5=cosr**5
cosrP6=cosr**6
cosrP7=cosr**7
cosrP8=cosr**8
cosrP9=cosr**9
cosbigR4=cosbigR**4

secrP2=secr**2
secrP3=secr**3
secrP4=secr**4
secrP5=secr**5
secrP6=secr**6
secrP7=secr**7
secrP8=secr**8

tmp1=Log(dcmplx((-4 + r2)/(4 + r2),0))
tmp2=Log(dcmplx(-r2/4.,0))
tmp3=Log(dcmplx(1 + ((-4 + r2)*cosr)/(4 + r2),0))
tmp4=Log(dcmplx(2 + (2*(-4 + r2)*cosr)/(4 + r2),0))
tmp5=Log(dcmplx(1 - secr,0))
tmp6=Log(dcmplx(-secr,0))
tmp7=Log(dcmplx((2*r2)/(-4 + r2),0))
tmp8=Log(dcmplx(0.5 - 2/r2,0))
tmp9=Log(dcmplx((4 + r2)/(-4 + r2),0))
tmp10=Log(dcmplx(1 - secbigR,0))
tmp11=Log(dcmplx(-secbigR,0))
tmp12=Log(dcmplx(-cosbigR,0))
tmp13=Log(dcmplx((-2*r2*(-1 + cosbigR))/(4 + r2),0))
tmp14=Log(dcmplx(-(((4 + r2)*(-1 + cosbigR))/(4 + r2 + (-4 + r2)*cosbigR)),0))
tmp15=Log(dcmplx((-2*r2*(-1 + cosr))/(4 + r2),0))
tmp16=Log(dcmplx((-2*r2)/(4 + r2),0))
tmp17=Log(dcmplx(-1 - ((-4 + r2)*cosbigR)/(4 + r2),0))
tmp18=Log(dcmplx(-cosr,0))

if(iparton.ne.21) then
include "Qint1Con.f90"
include "Qint1LogS1.f90"
include "Qint1LogS2sub1.f90"
include "Qint1LogS2sub2.f90"
include "Qint1LogS2sub3.f90"
include "Qint1LogS3sub1.f90"
include "Qint1LogS3sub2.f90"
include "Qint1LogS3sub3.f90"
include "Qint2Con.f90"
include "Qint2LogS1.f90"
include "Qint2LogS2.f90"
include "Qint2LogS3sub1.f90"
include "Qint2LogS3sub2.f90"
include "VirtualQuark.f90"
include "SudQuark.f90"

initQuark = &
Qint1Con + Qint2Con + Qint1LogS1 + Qint1LogS2sub1 + Qint1LogS2sub2 + &
Qint1LogS2sub3 + Qint1LogS3sub1 + Qint1LogS3sub2 + &
Qint1LogS3sub3 + Qint2LogS1 + Qint2LogS2 + Qint2LogS3sub1 + &
Qint2LogS3sub2 - (2*logr**2 - 3/2d0*logr)*CF*alphas/pi/ej

resum = real((1/ej + initQuark + VirtualQuark + correction) * Exp(SudQuark))
else
include "Gint1Con.f90"
include "Gint1LogS1.f90"
include "Gint1LogS2sub1.f90"
include "Gint1LogS2sub2.f90"
include "Gint1LogS2sub3.f90"
include "Gint1LogS3sub1.f90"
include "Gint1LogS3sub2.f90"
include "Gint2Con.f90"
include "Gint2LogS1.f90"
include "Gint2LogS2.f90"
include "Gint2LogS3sub1.f90"
include "Gint2LogS3sub2.f90"
include "VirtualGluon.f90"
include "SudGluon.f90"

initGluon = &
Gint1Con + Gint2Con + Gint1LogS1 + Gint1LogS2sub1 + Gint1LogS2sub2 + &
Gint1LogS2sub3 + Gint1LogS3sub1 + Gint1LogS3sub2 + Gint2LogS1 + &
Gint2LogS2 + Gint2LogS3sub1 + &
Gint2LogS3sub2 - (2*logr**2 - 11/6d0*logr)*CA*alphas/pi/ej

resum = real((1/ej + initGluon + VirtualGluon + correction) * Exp(SudGluon))
endif

return

end subroutine calc_terms

function myLog(xx)
implicit none
complex(8) :: myLog
real(8) :: xx
myLog=log(dcmplx(xx,0))
end function myLog

end module terms

