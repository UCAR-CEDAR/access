pro test_auth,user,password,ipaddress
;{
    r=cedar_authenticate_user(user, password)
    if (r eq 0) then begin ;{
	y=cedar_create_session(user, ipaddress)
	if (y eq 0) then begin ;{
	    print,'  + logged in successfully'
	endif else begin ;} ;{
	    print,'  - create session failed'
	endelse ;}
    endif else begin ;{
	print,'  - authentication failed'
    endelse ;}
end ;}
