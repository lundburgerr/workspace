function [h] = findIR_reverb(fs, R, L, room, mic, source, hrtfm)

%[h] = findIR_reverb(fs,R,L, room, mic, source, hrtf)
% performs a room impulse response calculation by means of the mirror image method.
%
%      fs     = sample rate.
%      mic    = row vector giving the x,y,z coordinates of the microphone.
%      L      = The required length of the impulse response in samples,
%      (note to big L will take extremly long time to calculate)
%
%      R      = reflection coefficient for the walls, in general -1<R<1.
%      room   = row vector giving the dimensions of the room.
%      source = row vector giving the x,y,z coordinates of
%            the sound source.
%      hrtfm  = multi-dimensional matrix with all HRTFs

v_sound=340; %speed of sound
h=zeros(L,1);

% there are 8 different "room reflection-images" (including the original room) in
% the "positive x, y z world"
abc=[0,0,0;
    0,0,1;
    0,1,0;
    0,1,1;
    1,0,0;
    1,0,1;
    1,1,0;
    1,1,1;];

%how many rooms to we have to calculate to fill the required length of the
%impulse response
Nx=ceil(L/2/(room(1)/v_sound*fs));
Ny=ceil(L/2/(room(2)/v_sound*fs));
Nz=ceil(L/2/(room(3)/v_sound*fs));

%find out where the first filter coefficient will appear. Then remove all
%zeros before this index since they will not contribute to the model exept
%for a long delay.
index_min=L;


for mx=-Nx:Nx
    for my=-Ny:Ny
        for mz=-Nz:Nz
            for i=1:8
                
                %reflected room image.
                rp=([1,1,1]-2*abc(i,:)).*source;
                
                %repeats of room images
                rm=2*[mx,my,mz].*room;
                
                %distance between microphone and source image.
                dist=norm(rp+rm-mic);

                %convert distance to index
                index=round(dist/v_sound*fs);
                index_min=min(index_min, index);
                if(index <= L)
                    %damping of the sound due to the distance travelled
                    dist_damp=1/(dist);
                    
                    %number of reflections
                    num_ref=abs(2*mx-abc(i,1))+abs(2*my-abc(i,2))+abs(2*mz-abc(i,3));
                    
                    %total reflection damping (assuming same reflection
                    %coefficient of all walls
                    R_tot= R^num_ref;

					% Vector of the arriving reflection
					svect = rp + rm - mic;
					% Azimut angle (degrees)
					az = atan2(svect(1), svect(2))/pi*180;
					% Elevation angle (degrees)
					el = asind(svect(3)/dist);

					% Elevation angle can only vary between -40 and 90
					% degrees
					el = max(el, -40);
					el = min(el, 90);

					% Indices in matrix
					az = round((az+180)/5);
					el = round((el+40)/10);
					% Get corresponding HRTF
					hrtf = hrtfm(:,az+1,el+1);
					% Resampe to Fs
%					hrtf = resample(hrtf, fs, 44100);
					lhrtf = length(hrtf);

					% Make sure we don't step outside the IR
					index_end = min(L, index + lhrtf -1);
					hrtf_len = min(lhrtf, index_end - index + 1);

					% add to the impulse response
					%h(index)=h(index)+R_tot*dist_damp;%
					h(index:index_end) = h(index:index_end) + hrtf(1:hrtf_len)*R_tot*dist_damp;

                end
            end
        end
    end
end
%remove all zeros at the beginning of the impulse response. (will change
%the total delay of the filter)
%if (index_min>1)
%    h=[h(index_min:end);zeros(index_min-1,1)];
%end



% figure
% t=[1/fs:1/fs:1];
% t=t(1:length(h));
% 
% subplot(2,2,1)
% plot(t,20*log10(abs(h)))
% hold on
% plot([t(1),t(end)],20*log10(abs([0.001,0.001])),'r')
% legend('impulse response', 'T60 threshold')
% xlabel('time (s)')
% ylabel('Amplitude (dB)')
% title('Logarithmic view of the impulse response')
% subplot(2,2,2)
% plot(t,h)
% 
% legend('impulse response')
% xlabel('time (s)')
% ylabel('Amplitude ')
% title('Impulse response')
% 
% subplot(2,2,3)
% plot(20*log10(abs(h)))
% hold on
% plot(20*log10(abs([0.001,0.001])),'r')
% legend('impulse response', 'T60 threshold')
% xlabel('samples')
% ylabel('Amplitude (dB)')
% title('Logarithmic view of the impulse response')
% subplot(2,2,4)
% plot(h)
% 
% legend('impulse response')
% xlabel('samples')
% ylabel('Amplitude ')
% title('Impulse response')