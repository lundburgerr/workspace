function varargout = DAtest(varargin)
% DATEST MATLAB code for DAtest.fig
%      DATEST, by itself, creates a new DATEST or raises the existing
%      singleton*.
%
%      H = DATEST returns the handle to a new DATEST or the handle to
%      the existing singleton*.
%
%      DATEST('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in DATEST.M with the given input arguments.
%
%      DATEST('Property','Value',...) creates a new DATEST or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before DAtest_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to DAtest_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help DAtest

% Last Modified by GUIDE v2.5 30-Jul-2013 11:28:10

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @DAtest_OpeningFcn, ...
                   'gui_OutputFcn',  @DAtest_OutputFcn, ...
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


% --- Executes just before DAtest is made visible.
function DAtest_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to DAtest (see VARARGIN)

% Choose default command line output for DAtest
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes DAtest wait for user response (see UIRESUME)
% uiwait(handles.figure1);

set(handles.rightText,'String',0);
set(handles.wrongText,'String',0);
set(handles.fracText,'String',strcat(num2str(100),'%'));
set(handles.errorText,'String','');

global Sound;
global Fs;
global CurretnFilterIndex;

[s, fs_r]  = wavread('../adaptive_gain_equalizer/2013-03-27 - Normal_Speech.wav');
Fs = 8000;
s = resample(s, Fs, fs_r);
Sound = s(1:Fs*2.9);
CurretnFilterIndex=0; %initialize

CreateFilteredSound();
PlaySound();

function CreateFilteredSound

global Fs;
global Sound;
global FilteredSound;
global CurretnFilterIndex;
global FiltersL;
global FiltersR;

newIndex = floor(rand()*5)+1;
while (newIndex == CurretnFilterIndex)
    newIndex = floor(rand()*5)+1;
end
CurretnFilterIndex = newIndex;

if CurretnFilterIndex==1
    FilteredSound = Sound;  
else
    sl = filter(FiltersL(CurretnFilterIndex-1,:),1,Sound);
    sr = filter(FiltersR(CurretnFilterIndex-1,:),1,Sound);
    FilteredSound = [sl,sr];
end

function PlaySound

global FilteredSound;
global Fs;
sound(FilteredSound, Fs);

% --- Outputs from this function are returned to the command line.
function varargout = DAtest_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in playAgainButton.
function playAgainButton_Callback(hObject, eventdata, handles)
% hObject    handle to playAgainButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
PlaySound()



% --- Executes on selection change in directionSelect.
function directionSelect_Callback(hObject, eventdata, handles)
% hObject    handle to directionSelect (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns directionSelect contents as cell array
%        contents{get(hObject,'Value')} returns selected item from directionSelect


% --- Executes during object creation, after setting all properties.
function directionSelect_CreateFcn(hObject, eventdata, handles)
% hObject    handle to directionSelect (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in DoneButton.
function DoneButton_Callback(hObject, eventdata, handles)
% hObject    handle to DoneButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global CurretnFilterIndex;

right = str2num(get(handles.rightText,'String'));
wrong = str2num(get(handles.wrongText,'String'));
%if correct
if CurretnFilterIndex == get(handles.directionSelect,'Value');
    set(handles.errorText,'String','');
    right = right + 1;
else
    wrong = wrong + 1;
    list=get(handles.directionSelect,'String');
    set(handles.errorText,'String',list(CurretnFilterIndex));
end

set(handles.rightText,'String',right);
set(handles.wrongText,'String',wrong);
set(handles.fracText,'String',strcat(num2str(100*right/(wrong+right)),'%'));

CreateFilteredSound();
PlaySound();
