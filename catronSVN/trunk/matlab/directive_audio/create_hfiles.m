%Creates directive audio filters and store them as .h files for C.  
function create_hfiles
[IRl_f IRr_f]=directive_filter(0, 0/180*pi, 8000);
[IRl_rf IRr_rf]=directive_filter(0, 45/180*pi, 8000);
[IRl_lf IRr_lf]=directive_filter(0, -45/180*pi, 8000);
[IRl_rb IRr_rb]=directive_filter(0, 90/180*pi, 8000);
[IRl_lb IRr_lb]=directive_filter(0, -90/180*pi, 8000);


data_name = 'pq15Front_coefs_left';
file_name='front_coefs_left';
data = IRl_f;
create_hfile(data,data_name,file_name);
data_name = 'pq15Front_coefs_right';
file_name='front_coefs_right';
data = IRr_f;
create_hfile(data,data_name,file_name);

data_name = 'pq15RightFront_coefs_left';
file_name='rightfront_coefs_left';
data = IRl_rf;
create_hfile(data,data_name,file_name);
data_name = 'pq15RightFront_coefs_right';
file_name='rightfront_coefs_right';
data = IRr_rf;
create_hfile(data,data_name,file_name);

data_name = 'pq15LeftFront_coefs_left';
file_name='leftfront_coefs_left';
data = IRl_lf;
create_hfile(data,data_name,file_name);
data_name = 'pq15LeftFront_coefs_right';
file_name='leftfront_coefs_right';
data = IRr_lf;
create_hfile(data,data_name,file_name);

data_name = 'pq15RightBack_coefs_left';
file_name='rightback_coefs_left';
data = IRl_rb;
create_hfile(data,data_name,file_name);
data_name = 'pq15RightBack_coefs_right';
file_name='rightback_coefs_right';
data = IRr_rb;
create_hfile(data,data_name,file_name);

data_name = 'pq15LeftBack_coefs_left';
file_name='leftback_coefs_left';
data = IRl_lb;
create_hfile(data,data_name,file_name);
data_name = 'pq15LeftBack_coefs_right';
file_name='leftback_coefs_right';
data = IRr_lb;
create_hfile(data,data_name,file_name);

verification = false;
if verification
   global FiltersL;
   global FiltersR;
   FiltersL = zeros(4,length(IRl_lf));
   FiltersR = zeros(4,length(IRr_lf));
   
   FiltersL(1,:)=IRl_lf;
   FiltersR(1,:)=IRr_lf;
   
   FiltersL(2,:)=IRl_rf;
   FiltersR(2,:)=IRr_rf;
   
   FiltersL(3,:)=IRl_lb;
   FiltersR(3,:)=IRr_lb;

   FiltersL(4,:)=IRl_rb;
   FiltersR(4,:)=IRr_rb;
   
   DAtest();
end

end

function create_hfile(data,data_name,file_name)
write_to_inc(round(data*32768), '', [file_name,'.inc']);
write_header_file_from_inc(...
    data_name,...
    'q15',... % Type
    'COEFS_LENGTH',...
    ['_',upper(file_name)],...
    length(data),...
    file_name);
% Remove .inc-file that was created by "write_float_to_inc()"
system(['del ',[file_name,'.inc']]);
end