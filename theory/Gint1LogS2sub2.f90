Gint1LogS2sub2=-((alphas*CA*(8 + 2*r**2 + 8*cosr - 2*r**2*cosr - r**2*log(4d0) +    &
        r**2*cosr * log(4d0) + log(256d0) + cosr * log(256d0) + 8*Log(r) -    &
        2*r**2*Log(r) + 8*cosr * Log(r) + 2*r**2*cosr * Log(r) +    &
        4*Log(1 - cosr) - r**2*Log(1 - cosr) +    &
        4*cosr * Log(1 - cosr) + r**2*cosr * Log(1 - cosr) -    &
        4*Log(4 + r**2 - (-4 + r**2)*cosr) +    &
        r**2*Log(4 + r**2 - (-4 + r**2)*cosr) -    &
        4*cosr * Log(4 + r**2 - (-4 + r**2)*cosr) -    &
        r**2*cosr * Log(4 + r**2 - (-4 + r**2)*cosr) +    &
        (-4 - r**2 + (-4 + r**2)*cosr)*   &
         mydilog(0.5 - ((0,1)*cotro2)/r) +    &
        (-4 - r**2 + (-4 + r**2)*cosr)*   &
         mydilog(0.5 + ((0,1)*cotro2)/r) -    &
        8*r*atan(((4 - r**2 + (4 + r**2)*cosr)*cscr)/(4.*r))*sinr -    &
        8*r*atan((r*tanro2)/2.)*sinr))/   &
    (ej*pi*(-4 - r**2 + (-4 + r**2)*cosr)))
