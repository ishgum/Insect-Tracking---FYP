f = figure ;
t = 0:0.01: pi/2 ;
polar(t, 10 * log10(cos(t))/(50) + 1)
rho_labels = {'1' '0.8' '0.6' '0.4' '0.2'};
rho_labels2 = {'0' 'dsf' '-20' '-30' '-40'};
ff = findall(f,'type','text');
t=strtrim(get(ff,'String'));
for r=1:length(rho_labels)
   set(ff(strcmp(t,rho_labels{r})),'String',rho_labels2{r})
end