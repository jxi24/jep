module constant
implicit none
real(8), parameter :: epsilon=1.d-5
real(8), parameter :: pi=3.141592653589793d0
real(8), parameter :: pi2=pi**2, pi3 = pi**2/3!, pi6 = pi**2/6, pi12 = pi**2/12
real(8), parameter :: one = 1.d0, half = one/2.d0
real(8), parameter :: CF=4.d0/3.d0
real(8), parameter :: CA=3.d0
real(8), parameter :: CFFAC=4.d0/3.d0
real(8), parameter :: CAFAC=3.d0
real(8), parameter :: geuler =0.577215664901533d0
real(8), parameter :: hbar_c2 = 3.8937966d8 ! = hbar c^2 in picobarn
real(8), parameter :: &
    &   sqrt2 = 1.41421356237309504880168872420969807856967187537694807317d0 &
    & , oosqrt2 = 1.d0 / sqrt2                                               &
    & , sqrt3 = 1.73205080756887729352744634150587236694280525381038062805d0 &
    & , oosqrt3 = 1.d0 / sqrt3
real(8), parameter :: degree = pi/180.d0
complex(8), parameter :: complexi=cmplx(0.d0,1.d0)
  real(8), parameter :: zeta2 = pi2 / 6.d0                    &   
   & , zeta3 = 1.202056903159594285399738161511449990765d0   &
   & , zeta4 = 1.082323233711138191516003696541167902775d0   &
   & , zeta5 = 1.036927755143369926331365486457034168057d0 &
   & , zeta6 = pi**6/945.d0
real(8), parameter :: beta05=11.d0-2.d0/3.d0*5
real(8), parameter :: beta15=(51.d0-19.d0/3.d0*5)
real(8), parameter :: beta25=(2857.d0-5033.d0/9.d0*5+325.d0/27.d0*5**2)/2.d0
real(8), parameter :: beta35=149753/6.d0+3564*zeta3-(1078361/162.d0+6508/27.d0*zeta3)*5&
        +(50065/162.d0+6472/81.d0*zeta3)*5**2+1093/729.d0*5**3
real(8), parameter :: KK5=CA*(67/18d0-pi**2/6d0)-5/9d0*5

!delta\rho=3*sm%GF*sm%mu2(3)/(8*pi2*sqrt2)*(1-2/3.d0*(pi2/3.d0+1)*RGEalphas(sm%mu2(3))/pi)
! 8.36574d-3
! correct alpha for Z propagator
!alpha_corr=sqrt2*sm%MZ2*sm%GF*(1+delta\rho)/(pi/sm%SWeff2/sm%CWeff2)
! 1.d0/127.704381d0
real(8), parameter :: alpha_corr=1.d0/127.704381d0
real(8), parameter :: alpha_MZ=1.d0/137.0359895d0
real(8), parameter :: alphas_MZ=0.118d0
real(8), parameter :: QCDCUT=1d0
real(8), parameter :: LambdaQCD=0.3
integer, parameter :: ifn=111,ispectrum=101,iferr=102
integer, parameter :: iscan=199
integer, parameter :: iread=103,iwrite=104,isave=105
integer, parameter :: igrid1=113,igrid2=114,igridre=115,igridim=116
integer, parameter :: ie605=117,ie866=118, ie866p=119, itevz=198
integer, parameter :: inlores=120,innlo=121
integer, parameter :: ikin=122
integer, parameter :: istat1=123,istat2=124,istat3=125,istat4=126
integer, parameter :: isub1=127,isub2=128,isub3=129,isub4=130
integer, parameter :: isub5=131,isub6=132,isub7=133,isub8=134
integer, parameter :: istat=135
integer, parameter :: itradres=136
integer, parameter :: inpres=137

integer, parameter :: verbose=1+16
!integer, parameter :: maxeval=20000
!integer, parameter :: nstart=10000
!integer, parameter :: nincrease=10000
!real(8), parameter :: relaccuracy=1.d-2

integer, parameter :: GRIDfileID=101 ! grid file ID
integer, parameter :: LHEfileID=131 !LHE file ID
integer, parameter :: INfileID=121 !input file ID
integer, parameter :: OUTfileID=111 !output file ID


!-------------------------------------------------
real(8), save :: r, bigR
real(8), save :: cosr, cosbigR
real(8), save :: sinr, sinbigR
real(8), save :: secr, secbigR
real(8), save :: cscr, cscbigR
real(8), save :: cotr, cotbigR
real(8), save :: cosro2, cosbigRo2
real(8), save :: sinro2, sinbigRo2
real(8), save :: tanro2, tanbigRo2
real(8), save :: cotro2, cotbigRo2
real(8), save :: r2, bigR2
real(8), save :: r4, bigR4
real(8), save :: r6, bigR6
real(8), save :: r8, bigR8
real(8), save :: r10, bigR10
real(8), save :: r12, bigR12
real(8), save :: r14, bigR14
real(8), save :: r16, bigR16
real(8), save :: r18, bigR18
real(8), save :: r20, bigR20
real(8), save :: r22, bigR22
real(8), save :: r24, bigR24
real(8), save :: r26, bigR26
real(8), save :: r28, bigR28
real(8), save :: r30, bigR30
real(8), save :: r32, bigR32
real(8), save :: r34, bigR34
real(8), save :: r36, bigR36
real(8), save :: logr, logbigR
real(8), save :: cosrP2, cosbigR2
real(8), save :: cosrP3, cosbigR3
real(8), save :: cosrP4, cosbigR4
real(8), save :: cosrP5, cosbigR5
real(8), save :: cosrP6, cosbigR6
real(8), save :: cosrP7, cosbigR7
real(8), save :: cosrP8, cosbigR8
real(8), save :: cosrP9, cosbigR9
real(8), save :: secrP2, secrP3, secrP4, secrP5, secrP6, secrP7, secrP8
real(8), save :: sin2r, cos2r, sin3r, cos3r
real(8), save :: sin2bigR, cos2bigR, sin3bigR, cos3bigR

real(8), save :: jetMass, jetPT, jetR, jetNx

complex(8), save :: Qint1Con, Qint2Con, Qint1LogS1
complex(8), save :: Qint1LogS2sub1, Qint1LogS2sub2, Qint1LogS2sub3 
complex(8), save :: Qint1LogS3sub1, Qint1LogS3sub2, Qint1LogS3sub3 
complex(8), save :: Qint2LogS1, Qint2LogS2
complex(8), save :: Qint2LogS3sub1, Qint2LogS3sub2 
complex(8), save :: SudQuark, VirtualQuark

complex(8), save :: Gint1Con, Gint1LogS1
complex(8), save :: Gint1LogS2sub1, Gint1LogS2sub2, Gint1LogS2sub3
complex(8), save :: Gint1LogS3sub1, Gint1LogS3sub2
complex(8), save :: Gint2Con, Gint2LogS1, Gint2LogS2
complex(8), save :: Gint2LogS3sub1, Gint2LogS3sub2
complex(8), save :: SudGluon, VirtualGluon

real(8), save :: ej
real(8), save :: alphas
real(8), save :: lamr, lambigR, lamC

complex(8), save :: tmp1, tmp2, tmp3, tmp4, tmp5
complex(8), save :: tmp6, tmp7, tmp8, tmp9, tmp10
complex(8), save :: tmp11, tmp12, tmp13, tmp14, tmp15
complex(8), save :: tmp16, tmp17, tmp18, tmp19, tmp20

real(8), parameter :: C1sq=1!exp(geuler)**2
real(8), parameter :: C2sq=1!exp(geuler)**2
real(8), save :: correction

contains

function alphasP(Q)
implicit none
real(8) :: alphasP, Q
real(8), parameter :: alphasMZ = 0.118
alphasP = alphasMZ/XX(Q)*(1 - alphasMZ/2d0/pi*beta15/beta05*Log(XX(Q))/XX(Q))
end function alphasP

function XX(Q)
implicit none
real(8) :: XX, Q
real(8), parameter :: alphasMZ = 0.118
real(8), parameter :: MZ = 91.187
XX=1 + alphasMZ/4d0/pi*beta05*Log(Q**2/MZ**2)
end function XX

recursive function mydilog(xx) result(res)
implicit none
complex(8) :: res, xx
complex(8) :: temp
integer :: nn
real(8) :: nnE
real(8) :: rate

if(abs(xx).gt.1d0) then
  res=-mydilog(1/xx)-1/2d0*log(-1/xx)**2-pi**2/6d0
  return
else if(abs(xx).eq.1d0) then
  res=pi**2/6d0
else 
  rate=1
  res=0
  nn=1
  do while(rate.gt.1d-4)
    temp=xx**nn/(nn*(nn+1)*(nn+2))**2
    res=res+temp
    rate=abs(temp/res)
    nn=nn+1
  enddo
  res=4*xx**2*res+4*xx+23/4d0*xx**2+3*(1-xx**2)*log(1-xx)
  res=res/(1+4*xx+xx**2)
  return
endif


end function mydilog

recursive function myDdilog(xx) result(res)
implicit none
complex(8) :: res
real(8) :: xx
complex(8) :: temp
integer :: nn
real(8) :: nnE
real(8) :: rate
real(8), external :: ddilog

if(abs(xx).gt.1d0) then
  res=-ddilog(1/xx)-1/2d0*log(-dcmplx(xx,0))**2-pi**2/6d0
  res=conjg(res)
  return
else 
  res=ddilog(xx)
!else if(abs(xx).eq.1d0) then
!  res=pi**2/6d0
!else 
!  rate=1
!  res=0
!  nn=1
!  do while(rate.gt.1d-4)
!    temp=xx**nn/(nn*(nn+1)*(nn+2))**2
!    res=res+temp
!    rate=abs(temp/res)
!    nn=nn+1
!  enddo
!  res=4*xx**2*res+4*xx+23/4d0*xx**2+3*(1-xx**2)*log(1-xx)
!  res=res/(1+4*xx+xx**2)
!  return
endif


end function myDdilog

end module constant
