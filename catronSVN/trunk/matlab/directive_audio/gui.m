function varargout = gui(varargin)
% GUI MATLAB code for gui.fig
%      GUI, by itself, creates a new GUI or raises the existing
%      singleton*.
%
%      H = GUI returns the handle to a new GUI or the handle to
%      the existing singleton*.
%
%      GUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in GUI.M with the given input arguments.
%
%      GUI('Property','Value',...) creates a new GUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before gui_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to gui_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help gui

% Last Modified by GUIDE v2.5 24-May-2013 15:06:15

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @gui_OpeningFcn, ...
                   'gui_OutputFcn',  @gui_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before gui is made visible.
function gui_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to gui (see VARARGIN)

% Choose default command line output for gui
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes gui wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = gui_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on slider movement.
function delay1_Callback(hObject, eventdata, handles)
% hObject    handle to delay1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global delay;
global changeAngles;
delay(1) = get(hObject, 'Value')*5; %Delay from 0 to 5 seconds
changeAngles(1) = true;
set(handles.delay1_text,'String',delay(1))

% --- Executes during object creation, after setting all properties.
function delay1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on button press in mute1.
function mute1_Callback(hObject, eventdata, handles)
% hObject    handle to mute1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of mute1
global mute;
global changeMute;
mute(1) = get(hObject, 'Value');
disp(mute(1))
changeMute(1) = true;


% --- Executes on slider movement.
function az1_Callback(hObject, eventdata, handles)
% hObject    handle to az1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global azimuthal;
global changeAngles;
azimuthal(1) = get(hObject, 'Value')*360-180; %Going from -180 to 180 degrees
azimuthal(1) = round(azimuthal(1)/5)*5; % Azimuth angle (degrees from -180 to 180, steps of 5)
set(handles.az1_text,'String',azimuthal(1))
azimuthal(1) = azimuthal(1)*2*pi/360; %Convert to radians
changeAngles(1) = true;



% --- Executes during object creation, after setting all properties.
function az1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to az1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function el1_Callback(hObject, eventdata, handles)
% hObject    handle to el1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global elevation;
global changeAngles;
elevation(1) = get(hObject, 'Value')*130-40; %Going from -40 to 90 degrees
elevation(1) = round(elevation(1)/10)*10; % Elevation angle (degrees from -40 to 90, steps of 10)
set(handles.el1_text,'String',elevation(1))
elevation(1) = elevation(1)*2*pi/360; %Convert to radians
changeAngles(1) = true;

% --- Executes during object creation, after setting all properties.
function el1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to el1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end



function delay1_text_Callback(hObject, eventdata, handles)
% hObject    handle to delay1_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of delay1_text as text
%        str2double(get(hObject,'String')) returns contents of delay1_text as a double


% --- Executes during object creation, after setting all properties.
function delay1_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay1_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function az1_text_Callback(hObject, eventdata, handles)
% hObject    handle to az1_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of az1_text as text
%        str2double(get(hObject,'String')) returns contents of az1_text as a double


% --- Executes during object creation, after setting all properties.
function az1_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to az1_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function el1_text_Callback(hObject, eventdata, handles)
% hObject    handle to el1_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of el1_text as text
%        str2double(get(hObject,'String')) returns contents of el1_text as a double


% --- Executes during object creation, after setting all properties.
function el1_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to el1_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object deletion, before destroying properties.
function figure1_DeleteFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global exit;
exit = true;


% --- Executes when user attempts to close figure1.
function figure1_CloseRequestFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: delete(hObject) closes the figure
delete(hObject);


% --- Executes on slider movement.
function delay2_Callback(hObject, eventdata, handles)
% hObject    handle to delay2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global delay;
global changeAngles;
delay(2) = get(hObject, 'Value')*5; %Delay from 0 to 5 seconds
changeAngles(2) = true;
set(handles.delay2_text,'String',delay(2))


% --- Executes during object creation, after setting all properties.
function delay2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on button press in mute2.
function mute2_Callback(hObject, eventdata, handles)
% hObject    handle to mute2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of mute2
global mute;
global changeMute;
mute(2) = get(hObject, 'Value');
changeMute(2) = true;


% --- Executes on slider movement.
function az2_Callback(hObject, eventdata, handles)
% hObject    handle to az2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global azimuthal;
global changeAngles;
azimuthal(2) = get(hObject, 'Value')*360-180; %Going from -180 to 180 degrees
azimuthal(2) = round(azimuthal(2)/5)*5; % Azimuth angle (degrees from -180 to 180, steps of 5)
set(handles.az2_text,'String',azimuthal(2))
azimuthal(2) = azimuthal(2)*2*pi/360; %Convert to radians
changeAngles(2) = true;


% --- Executes during object creation, after setting all properties.
function az2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to az2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function el2_Callback(hObject, eventdata, handles)
% hObject    handle to el2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global elevation;
global changeAngles;
elevation(2) = get(hObject, 'Value')*130-40; %Going from -40 to 90 degrees
elevation(2) = round(elevation(2)/10)*10; % Elevation angle (degrees from -40 to 90, steps of 10)
set(handles.el2_text,'String',elevation(2))
elevation(2) = elevation(2)*2*pi/360; %Convert to radians
changeAngles(2) = true;


% --- Executes during object creation, after setting all properties.
function el2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to el2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end



function delay2_text_Callback(hObject, eventdata, handles)
% hObject    handle to delay2_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of delay2_text as text
%        str2double(get(hObject,'String')) returns contents of delay2_text as a double


% --- Executes during object creation, after setting all properties.
function delay2_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay2_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function az2_text_Callback(hObject, eventdata, handles)
% hObject    handle to az2_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of az2_text as text
%        str2double(get(hObject,'String')) returns contents of az2_text as a double


% --- Executes during object creation, after setting all properties.
function az2_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to az2_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function el2_text_Callback(hObject, eventdata, handles)
% hObject    handle to el2_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of el2_text as text
%        str2double(get(hObject,'String')) returns contents of el2_text as a double


% --- Executes during object creation, after setting all properties.
function el2_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to el2_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on slider movement.
function delay3_Callback(hObject, eventdata, handles)
% hObject    handle to delay3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global delay;
global changeAngles;
delay(3) = get(hObject, 'Value')*5; %Delay from 0 to 5 seconds
changeAngles(3) = true;
set(handles.delay3_text,'String',delay(3))


% --- Executes during object creation, after setting all properties.
function delay3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on button press in mute3.
function mute3_Callback(hObject, eventdata, handles)
% hObject    handle to mute3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of mute3
global mute;
global changeMute;
mute(3) = get(hObject, 'Value');
changeMute(3) = true;

% --- Executes on slider movement.
function az3_Callback(hObject, eventdata, handles)
% hObject    handle to az3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global azimuthal;
global changeAngles;
azimuthal(3) = get(hObject, 'Value')*360-180; %Going from -180 to 180 degrees
azimuthal(3) = round(azimuthal(3)/5)*5; % Azimuth angle (degrees from -180 to 180, steps of 5)
set(handles.az3_text,'String',azimuthal(3))
azimuthal(3) = azimuthal(3)*2*pi/360; %Convert to radians
changeAngles(3) = true;


% --- Executes during object creation, after setting all properties.
function az3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to az3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function el3_Callback(hObject, eventdata, handles)
% hObject    handle to el3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global elevation;
global changeAngles;
elevation(3) = get(hObject, 'Value')*130-40; %Going from -40 to 90 degrees
elevation(3) = round(elevation(3)/10)*10; % Elevation angle (degrees from -40 to 90, steps of 10)
set(handles.el3_text,'String',elevation(3))
elevation(3) = elevation(3)*2*pi/360; %Convert to radians
changeAngles(3) = true;


% --- Executes during object creation, after setting all properties.
function el3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to el3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end



function delay3_text_Callback(hObject, eventdata, handles)
% hObject    handle to delay3_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of delay3_text as text
%        str2double(get(hObject,'String')) returns contents of delay3_text as a double


% --- Executes during object creation, after setting all properties.
function delay3_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay3_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function az3_text_Callback(hObject, eventdata, handles)
% hObject    handle to az3_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of az3_text as text
%        str2double(get(hObject,'String')) returns contents of az3_text as a double


% --- Executes during object creation, after setting all properties.
function az3_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to az3_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function el3_text_Callback(hObject, eventdata, handles)
% hObject    handle to el3_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of el3_text as text
%        str2double(get(hObject,'String')) returns contents of el3_text as a double


% --- Executes during object creation, after setting all properties.
function el3_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to el3_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on slider movement.
function delay4_Callback(hObject, eventdata, handles)
% hObject    handle to delay4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global delay;
global changeAngles;
delay(4) = get(hObject, 'Value')*5; %Delay from 0 to 5 seconds
changeAngles(4) = true;
set(handles.delay4_text,'String',delay(4))


% --- Executes during object creation, after setting all properties.
function delay4_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on button press in mute4.
function mute4_Callback(hObject, eventdata, handles)
% hObject    handle to mute4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of mute4
global mute;
global changeMute;
mute(4) = get(hObject, 'Value');
changeMute(4) = true;


% --- Executes on slider movement.
function az4_Callback(hObject, eventdata, handles)
% hObject    handle to az4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global azimuthal;
global changeAngles;
azimuthal(4) = get(hObject, 'Value')*360-180; %Going from -180 to 180 degrees
azimuthal(4) = round(azimuthal(4)/5)*5; % Azimuth angle (degrees from -180 to 180, steps of 5)
set(handles.az4_text,'String',azimuthal(4))
azimuthal(4) = azimuthal(4)*2*pi/360; %Convert to radians
changeAngles(4) = true;


% --- Executes during object creation, after setting all properties.
function az4_CreateFcn(hObject, eventdata, handles)
% hObject    handle to az4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function el4_Callback(hObject, eventdata, handles)
% hObject    handle to el4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global elevation;
global changeAngles;
elevation(4) = get(hObject, 'Value')*130-40; %Going from -40 to 90 degrees
elevation(4) = round(elevation(4)/10)*10; % Elevation angle (degrees from -40 to 90, steps of 10)
set(handles.el4_text,'String',elevation(4))
elevation(4) = elevation(4)*2*pi/360; %Convert to radians
changeAngles(4) = true;


% --- Executes during object creation, after setting all properties.
function el4_CreateFcn(hObject, eventdata, handles)
% hObject    handle to el4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end



function delay4_text_Callback(hObject, eventdata, handles)
% hObject    handle to delay4_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of delay4_text as text
%        str2double(get(hObject,'String')) returns contents of delay4_text as a double


% --- Executes during object creation, after setting all properties.
function delay4_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay4_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function az4_text_Callback(hObject, eventdata, handles)
% hObject    handle to az4_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of az4_text as text
%        str2double(get(hObject,'String')) returns contents of az4_text as a double


% --- Executes during object creation, after setting all properties.
function az4_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to az4_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function el4_text_Callback(hObject, eventdata, handles)
% hObject    handle to el4_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of el4_text as text
%        str2double(get(hObject,'String')) returns contents of el4_text as a double


% --- Executes during object creation, after setting all properties.
function el4_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to el4_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on slider movement.
function delay5_Callback(hObject, eventdata, handles)
% hObject    handle to delay5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global delay;
global changeAngles;
delay(5) = get(hObject, 'Value')*5; %Delay from 0 to 5 seconds
changeAngles(5) = true;
set(handles.delay5_text,'String',delay(5))


% --- Executes during object creation, after setting all properties.
function delay5_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on button press in mute5.
function mute5_Callback(hObject, eventdata, handles)
% hObject    handle to mute5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of mute5
global mute;
global changeMute;
mute(5) = get(hObject, 'Value');
changeMute(5) = true;


% --- Executes on slider movement.
function az5_Callback(hObject, eventdata, handles)
% hObject    handle to az5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global azimuthal;
global changeAngles;
azimuthal(5) = get(hObject, 'Value')*360-180; %Going from -180 to 180 degrees
azimuthal(5) = round(azimuthal(5)/5)*5; % Azimuth angle (degrees from -180 to 180, steps of 5)
set(handles.az5_text,'String',azimuthal(5))
azimuthal(5) = azimuthal(5)*2*pi/360; %Convert to radians
changeAngles(5) = true;


% --- Executes during object creation, after setting all properties.
function az5_CreateFcn(hObject, eventdata, handles)
% hObject    handle to az5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function el5_Callback(hObject, eventdata, handles)
% hObject    handle to el5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global elevation;
global changeAngles;
elevation(5) = get(hObject, 'Value')*130-40; %Going from -40 to 90 degrees
elevation(5) = round(elevation(5)/10)*10; % Elevation angle (degrees from -40 to 90, steps of 10)
set(handles.el5_text,'String',elevation(5))
elevation(5) = elevation(5)*2*pi/360; %Convert to radians
changeAngles(5) = true;


% --- Executes during object creation, after setting all properties.
function el5_CreateFcn(hObject, eventdata, handles)
% hObject    handle to el5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end



function delay5_text_Callback(hObject, eventdata, handles)
% hObject    handle to delay5_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of delay5_text as text
%        str2double(get(hObject,'String')) returns contents of delay5_text as a double


% --- Executes during object creation, after setting all properties.
function delay5_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay5_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function az5_text_Callback(hObject, eventdata, handles)
% hObject    handle to az5_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of az5_text as text
%        str2double(get(hObject,'String')) returns contents of az5_text as a double


% --- Executes during object creation, after setting all properties.
function az5_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to az5_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function el5_text_Callback(hObject, eventdata, handles)
% hObject    handle to el5_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of el5_text as text
%        str2double(get(hObject,'String')) returns contents of el5_text as a double


% --- Executes during object creation, after setting all properties.
function el5_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to el5_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
