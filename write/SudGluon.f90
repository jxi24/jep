C=Exp(17d0/6d0)
b0=beta05/4d0/pi
lamC=alphas*b0*Log(C)
lamr=alphas*b0*Log(C1sq*r**2)
lambigR=alphas*b0*Log(C2sq*bigR2)

!alphas=alphasPDF(ej)
SudGluon = &
  4*pi*CA/alphas/beta05**2*( &
    2*(1+lamC+lamr+lambigR)*log(1+lamC+lamr+lambigR) &
    -2*(1+lamC+2*lamr)*log(1+lamC+2*lamr) &
    +(1+2*lamC+2*lamr)*log(1+2*lamC+2*lamr) &
    -(1+2*lamC+2*lambigR)*log(1+2*lamC+2*lambigR) ) &
  +beta15*CA/beta05**3*( &
    2*log(1+lamC+lamr+lambigR)*(log(1+lamC+lamr+lambigR)+2) &
    -2*log(1+lamC+2*lamr)*(log(1+lamC+2*lamr)+2) &
    +log(1+2*lamC+2*lamr)*(log(1+2*lamC+2*lamr)+2) &
    -log(1+2*lamC+2*lambigR)*(log(1+2*lamC+2*lambigR)+2) ) &
  +2*CA/beta05**2*(beta05*log(C2sq/C1sq)-2*KK5)*( &
    log(1+lamC+lamr+lambigR)-log(1+lamC+2*lamr) &
    +0.5*log(1+2*lamC+2*lamr)-0.5*log(1+2*lamC+2*lambigR) ) &
  +CA/beta05*(1+log(C2sq/C1sq))*( &
    log(1+2*lamC+lambigR)-log(1+2*lamC+2*lamr-lambigR) )

!alphas=alphasPDF(sqrt(C2sq)*bigR*ej)
!SudGluon = &
!  4*pi*CA/alphas/beta05**2*( &
!    -2*(1+lamC+2*lamr-lambigR)*log(1+lamC+2*lamr-lambigR) &
!    +(1+2*lamC+2*lamr-lambigR)*log(1+2*lamC+2*lamr-lambigR) &
!    +2*(1+lamC+lamr)*log(1+lamC+lamr) &
!    -(1+2*lamC+lambigR)*log(1+2*lamC+lambigR) ) &
!  +beta15*CA/beta05**3*( &
!    -2*log(1+lamC+2*lamr-lambigR)*(log(1+lamC+2*lamr-lambigR)+2) &
!    +(log(1+2*lamC+2*lamr-lambigR)-log(1+2*lamC+lambigR)) &
!     *(log(1+2*lamC+2*lamr-lambigR)+log(1+2*lamC+lambigR)+2) &
!    +2*log(1+lamC+lamr)*(log(1+lamC+lamr)+2) ) &
!  +CA/beta05**2*(beta05*log(C2sq/C1sq)-2*KK5)*( &
!    -2*log(1+lamC+2*lamr-lambigR)+log(1+2*lamC+2*lamr-lambigR) &
!    -log(1+2*lamC+lambigR)+2*log(1+lamC+lamr) ) &
!  +CA/beta05*(1+log(C2sq/C1sq))*( &
!    log(1+2*lamC)-log(1+2*lamC+2*lamr-2*lambigR) )

!alphas=alphasPDF(ej)
correction=0.5*(1-log(C))*log(C1sq/C2sq)-0.25*log(C1sq/C2sq)**2
correction=correction*CA*alphas/pi/ej
    

!SudGluon=(CA*(-4*(alphas*(beta15 - beta05*KK5) + 2*beta05*(1 + lamC + 2*lamr)*pi)*  &
!       Log(1 + lamC + 2*lamr) - 2*alphas*beta15*Log(1 + lamC + 2*lamr)**2 +   &
!      (-(alphas*(beta05**2 - 2*beta15 + 2*beta05*KK5)) +   &
!         4*beta05*(1 + 2*lamC + 2*lamr)*pi)*Log(1 + 2*(lamC + lamr)) +   &
!      alphas*beta15*Log(1 + 2*(lamC + lamr))**2 +   &
!      2*Log(1 + lamC + lamr + lambigR)*  &
!       (2*alphas*(beta15 - beta05*KK5) +   &
!         4*beta05*(1 + lamC + lamr + lambigR)*pi +   &
!         alphas*beta15*Log(1 + lamC + lamr + lambigR)) +   &
!      (alphas*(beta05**2 - 2*beta15 + 2*beta05*KK5) -   &
!         4*beta05*(1 + 2*lamC + 2*lambigR)*pi)*Log(1 + 2*(lamC + lambigR)) -   &
!      alphas*beta15*Log(1 + 2*(lamC + lambigR))**2))/(alphas*beta05**3)
