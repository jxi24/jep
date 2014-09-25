Qint1LogS2sub2=-((alphas*CF*(8 + 2*r2 + 8*cosr - 2*r2*cosr - r2*log(4d0) +     &
        r2*cosr * log(4d0) + log(256d0) + cosr * log(256d0) + 8*logr -     &
        2*r2*logr + 8*cosr * logr + 2*r2*cosr * logr +     &
        4*Log(1 - cosr) - r2*Log(1 - cosr) +     &
        4*cosr * Log(1 - cosr) + r2*cosr * Log(1 - cosr) -     &
        4*Log(4 + r2 - (-4 + r2)*cosr) +     &
        r2*Log(4 + r2 - (-4 + r2)*cosr) -     &
        4*cosr * Log(4 + r2 - (-4 + r2)*cosr) -     &
        r2*cosr * Log(4 + r2 - (-4 + r2)*cosr) +     &
        (-4 - r2 + (-4 + r2)*cosr)*    &
         mydilog(0.5 - ((0,1)*cotro2)/r) +     &
        (-4 - r2 + (-4 + r2)*cosr)*    &
         mydilog(0.5 + ((0,1)*cotro2)/r) -     &
        8*r*atan(((4 - r2 + (4 + r2)*cosr)*cscr)/(4.*r))*sinr -     &
        8*r*atan((r*tanro2)/2.)*sinr))/    &
    (ej*pi*(-4 - r2 + (-4 + r2)*cosr)))
