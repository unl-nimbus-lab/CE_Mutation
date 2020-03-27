clear all
close all

chfl='/home/balaji/clang-llvm/mutationdocker/code/mutants_b747cl';
% Get a list of all files and folders in this folder.
dir_files = dir(chfl);
% Get a logical vector that tells which is a directory.
dirFlags = [dir_files.isdir];
% Extract only those that are directories.
folder_names = dir_files(dirFlags);

xaxisdata=1;
yaxisdata=1;
for no_of_step_quantities_above_threshold = 1:8
%Print in console the output of # live and # dead mutants
fprintf('th_percent \t sum(live_mutants) \t sum(dead_mutants)  \n');


for i = 3:length(folder_names)
    xlabel_dataout{i} = getfield(folder_names(i),'name');
end

%load base values to compare with
load('/home/balaji/clang-llvm/mutationdocker/code/mutants_b747cl/b747cl_original.mat')
base_value= rt_airspeed;
base_sti = stepinfo(rt_altitude,rt_t,62);

hi_th=100;
lo_th=0;
even_space_multiples= 11; %11
fig_index=1;
fig_row=3;

for th_loop=linspace(lo_th,hi_th,even_space_multiples)

folder_index=1;
mutant_index=1;

running_mutants_per_order = [];
blowing_mutants_per_order = [];

mutant_templates = [];
step_charac_RiseTime = [];
step_charac_SettlingTime = [];
step_charac_SettlingMin = [];
step_charac_SettlingMax = [];
step_charac_Overshoot = [];
step_charac_Undershoot = [];
step_charac_Peak = [];
step_charac_PeakTime = [];

%four characterization models for airspeed
airspeed_error_m1 = [];
airspeed_error_m2 = [];
airspeed_error_m3 = [];
airspeed_error_m4 = [];

%four characterization models for airspeed for non blowing models only
airspeed_error_non_blow_m1 = [];
airspeed_error_non_blow_m2 = [];
airspeed_error_non_blow_m3 = [];
airspeed_error_non_blow_m4 = [];

x_index =1;
all_mutants = [];
no_of_templates=length(folder_names)-2;
live_mutants = zeros(1,no_of_templates);
dead_mutants = zeros(1,no_of_templates);
dead_mutants_param = zeros(8,no_of_templates);
live_mutants_param = zeros(8,no_of_templates);
xin = 0:(no_of_templates-1);
total_mutants = 0;

threshold_value=th_loop; %9.096422555085;
threshold_percenage=threshold_value/100; %10 percentage
threshold_percenage
threshold_RiseTime = abs(threshold_value*base_sti.RiseTime); % base_sti.RiseTime  value is zero 
threshold_SettlingTime = abs(threshold_value*base_sti.SettlingTime); % base_sti.SettlingTime value is Nan
threshold_SettlingMin = abs(threshold_percenage * base_sti.SettlingMin);
threshold_SettlingMax = abs(threshold_percenage * base_sti.SettlingMax);
threshold_Overshoot = abs(threshold_percenage * base_sti.Overshoot);
threshold_Undershoot = abs(threshold_value*base_sti.Undershoot); % base_sti.Undershoot value is Nan
threshold_Peak = abs(threshold_percenage * base_sti.Peak);
threshold_PeakTime = abs(threshold_percenage * base_sti.PeakTime);
% threshold_RiseTime = threshold_value; % base_sti.RiseTime  value is zero 
% threshold_SettlingTime = threshold_value; % base_sti.SettlingTime value is Nan
% threshold_SettlingMin = threshold_percenage * base_sti.SettlingMin;
% threshold_SettlingMax = threshold_percenage * base_sti.SettlingMax;
% threshold_Overshoot = threshold_percenage * base_sti.Overshoot;
% threshold_Undershoot = threshold_value; % base_sti.Undershoot value is Nan
% threshold_Peak = threshold_percenage * base_sti.Peak;
% threshold_PeakTime = threshold_percenage * base_sti.PeakTime;

start_idx=1;
end_idx=0;

for folder_list = 3:numel(folder_names)
    mut_folder=strcat('/home/balaji/clang-llvm/mutationdocker/code/mutants_b747cl/',folder_names(folder_list).name);
    %mut_folder %print for debug
    
    cd(mut_folder)
    listoffiles = dir('*.c');
    C_nooffiles = numel(listoffiles);
    listoffiles = dir('*.txt');
    TXT_nooffiles = numel(listoffiles);
    listoffiles = dir('*.*');
    ALL_nooffiles = numel(listoffiles);
    log_txt_files=dir('log2_*.txt');
    
    nooffiles=C_nooffiles;
    %nooffiles %print for debug
    
    all_mutants(x_index)=nooffiles;
    total_mutants = total_mutants + nooffiles;

    %make sure we are at the right place before the simulation starts
    home = '/home/balaji/clang-llvm/mutationdocker/code/mutants_b747cl';
    cd(home)

    format long;
    %load base values to compare with
    load('/home/balaji/clang-llvm/mutationdocker/code/mutants_b747cl/b747cl_original.mat')
    base_value = rt_airspeed;

    %Error order
    error_order = 0;
    blow_mutant_count = 0;
    running_mutant_count = 0;

    %four characterization models for airspeed
    airspeed_error_m1_order = 0;
    airspeed_error_m2_order = 0;
    airspeed_error_m3_order = 0;
    airspeed_error_m4_order = 0;

    %four characterization models for airspeed for non blowing models only
    airspeed_error_non_blow_m1_order = 0;
    airspeed_error_non_blow_m2_order = 0;
    airspeed_error_non_blow_m3_order = 0;
    airspeed_error_non_blow_m4_order = 0;

    end_idx=end_idx+nooffiles;
    
    %for mutant 1 to 200
    for i = 1:numel(log_txt_files)

        %Generate right file paths
        file_path = mut_folder;
        fid = fopen([file_path '/' log_txt_files(i).name],'r');
        
        %fid = fopen([file_path '/log2_' int2str(i) '.txt'],'r');
        if fid == -1
            error('Author:Function:OpenFile', 'Cannot open file: %s', [file_path '/log2_' int2str(i) '.txt']);
        end
%         log= textscan(fid,'%s'); 
        
        log= textscan(fid,'%s');
        log = log{1};
        %if the mutant failed execution/or was not compiled successfully
        if (strcmp(log{1}, 'NO')) 
%             step_charac_RiseTime(i)= -1000;
%             step_charac_SettlingTime(i)= -1000;
%             step_charac_SettlingMin(i)= -1000;
%             step_charac_SettlingMax(i)= -1000;
%             step_charac_Overshoot(i)= -1000;
%             step_charac_Undershoot(i)= -1000;
%             step_charac_Peak(i)= -1000;
%             step_charac_PeakTime(i)= -1000;
            continue 
        end
        fclose(fid);
        
        [log_filepath,log_name,log_ext]=fileparts(log_txt_files(i).name);
        log_fileid = strsplit(log_name,'_');
        
        file = [file_path '/b747cl_' log_fileid{2} '.mat'];
%         file = [file_path '/b747cl_' int2str(i) '.mat'];
        load(file)
        sti = stepinfo(rt_airspeed,rt_t,151.148);

        %increase mutant count
        running_mutant_count = running_mutant_count + 1;
        is_blow_up = false;
        
        overall_index = mutant_index + (folder_index-1);
%         overall_index
%         mutant_templates(overall_index)=str2num(folder_names(folder_list).name);
        
        step_charac_RiseTime(overall_index)= sti.RiseTime;
        step_charac_SettlingTime(overall_index)= sti.SettlingTime;
        step_charac_SettlingMin(overall_index)= sti.SettlingMin;
        step_charac_SettlingMax(overall_index)= sti.SettlingMax;
        step_charac_Overshoot(overall_index)= sti.Overshoot;
        step_charac_Undershoot(overall_index)= sti.Undershoot;
        step_charac_Peak(overall_index)= sti.Peak;
        step_charac_PeakTime(overall_index)= sti.PeakTime;

        %Determine live or dead mutants
        count_th_exceeding=0;
        no_of_quantites_within_threshold=0;
        if (~is_blow_up) && (abs(sti.RiseTime-base_sti.RiseTime) > threshold_RiseTime)
%             is_blow_up = true;
            count_th_exceeding=count_th_exceeding+1;
            dead_mutants_param(1,x_index)=dead_mutants_param(1,x_index)+1;
        else
            no_of_quantites_within_threshold=no_of_quantites_within_threshold+1;
            live_mutants_param(1,x_index)=live_mutants_param(1,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && (abs(sti.SettlingTime-base_sti.SettlingTime) > threshold_SettlingTime)
%             is_blow_up = true;
            count_th_exceeding=count_th_exceeding+1;
            dead_mutants_param(2,x_index)=dead_mutants_param(2,x_index)+1;
        else
            no_of_quantites_within_threshold=no_of_quantites_within_threshold+1;
            live_mutants_param(2,x_index)=live_mutants_param(2,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && (abs(sti.SettlingMin-base_sti.SettlingMin) > threshold_SettlingMin)
%             is_blow_up = true;
            count_th_exceeding=count_th_exceeding+1;
            dead_mutants_param(3,x_index)=dead_mutants_param(3,x_index)+1;
        else
            no_of_quantites_within_threshold=no_of_quantites_within_threshold+1;
            live_mutants_param(3,x_index)=live_mutants_param(3,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && (abs(sti.SettlingMax-base_sti.SettlingMax) > threshold_SettlingMax)
%             is_blow_up = true;
            count_th_exceeding=count_th_exceeding+1;
            dead_mutants_param(4,x_index)=dead_mutants_param(4,x_index)+1;
        else
            no_of_quantites_within_threshold=no_of_quantites_within_threshold+1;
            live_mutants_param(4,x_index)=live_mutants_param(4,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && ( sti.Overshoot > (base_sti.Overshoot + threshold_Overshoot) )
%             is_blow_up = true;
            count_th_exceeding=count_th_exceeding+1;
            dead_mutants_param(5,x_index)=dead_mutants_param(5,x_index)+1;
        else
            no_of_quantites_within_threshold=no_of_quantites_within_threshold+1;
            live_mutants_param(5,x_index)=live_mutants_param(5,x_index)+1;
        end
        
%         is_blow_up
        if (~is_blow_up) && ( sti.Undershoot > (base_sti.Undershoot + threshold_Undershoot)  )
%             is_blow_up = true;
            count_th_exceeding=count_th_exceeding+1;
            dead_mutants_param(6,x_index)=dead_mutants_param(6,x_index)+1;
        else
            no_of_quantites_within_threshold=no_of_quantites_within_threshold+1;
            live_mutants_param(6,x_index)=live_mutants_param(6,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && (abs(sti.Peak-base_sti.Peak) > threshold_Peak)
%             is_blow_up = true;
            count_th_exceeding=count_th_exceeding+1;
            dead_mutants_param(7,x_index)=dead_mutants_param(7,x_index)+1;
        else
            no_of_quantites_within_threshold=no_of_quantites_within_threshold+1;
            live_mutants_param(7,x_index)=live_mutants_param(7,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && (abs(sti.PeakTime-base_sti.PeakTime) > threshold_PeakTime)
%             is_blow_up = true;   
            count_th_exceeding=count_th_exceeding+1;
            dead_mutants_param(8,x_index)=dead_mutants_param(8,x_index)+1;
        else
            no_of_quantites_within_threshold=no_of_quantites_within_threshold+1;
            live_mutants_param(8,x_index)=live_mutants_param(8,x_index)+1;
        end
        
%         is_blow_up
%         if(is_blow_up)
%             dead_mutants(x_index) = dead_mutants(x_index) + 1;
%         else
%             live_mutants(x_index) = live_mutants(x_index) + 1;
%         end
        
        if no_of_quantites_within_threshold >= no_of_step_quantities_above_threshold
            live_mutants(x_index) = live_mutants(x_index) + 1;
        else
            dead_mutants(x_index) = dead_mutants(x_index) + 1;
        end
        
        mutant_index = mutant_index+1;
    end
    
    start_idx = start_idx + nooffiles;
    
    folder_index = mutant_index + 1;
    mutant_index = 1;
    x_index = x_index+1;
%     step_charac_RiseTime

%     x=1:1:nooffiles;
    %plot(x,step_charac_RiseTime,x,step_charac_SettlingTime,x,step_charac_SettlingMin,x,step_charac_SettlingMax,x,step_charac_Overshoot,x,step_charac_Undershoot,x,step_charac_Peak,x,step_charac_PeakTime);

%     fig = figure;
%     hold on 
%     plot(x,step_charac_RiseTime,'bx',x,sti.RiseTime,'ro')
%     title('step charac RiseTime')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_SettlingTime,'bx',x,sti.SettlingTime,'ro')
%     title('step charac SettlingTime')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_SettlingMin,'bx',x,sti.RiseTime,'ro')
%     title('step charac SettlingMin')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_SettlingMax,'bx',x,sti.SettlingMax,'ro')
%     title('step charac SettlingMax')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_Overshoot,'bx',x,sti.Overshoot,'ro')
%     title('step charac Overshoot')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_Undershoot,'bx',x,sti.Undershoot,'ro')
%     title('step charac Undershoot')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_Peak,'bx',x,sti.Peak,'ro')
%     title('step charac Peak')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_PeakTime,'bx',x,sti.PeakTime,'ro')
%     title('step charac PeakTime')
%     hold off

end



mutant_templates(44)=44;
        
step_charac_RiseTime(44)= base_sti.RiseTime;
step_charac_SettlingTime(44)= base_sti.SettlingTime;
step_charac_SettlingMin(44)= base_sti.SettlingMin;
step_charac_SettlingMax(44)= base_sti.SettlingMax;
step_charac_Overshoot(44)= base_sti.Overshoot;
step_charac_Undershoot(44)= base_sti.Undershoot;
step_charac_Peak(44)= base_sti.Peak;
step_charac_PeakTime(44)= base_sti.PeakTime;

% figure;
% hold on 
% boxplot(step_charac_RiseTime,mutant_templates)
% title('Boxplot of step charac RiseTime')
% ylabel('RiseTime')
% xlabel('Mutant templates')
% hold off
% 
% figure;
% hold on 
% boxplot(step_charac_SettlingTime,mutant_templates)
% title('Boxplot of step charac SettlingTime')
% ylabel('SettlingTime')
% xlabel('Mutant templates')
% hold off
% 
% figure;
% hold on 
% boxplot(step_charac_SettlingMin,mutant_templates)
% title('Boxplot of step charac SettlingMin')
% ylabel('SettlingMin')
% xlabel('Mutant templates')
% hold off
% 
% figure;
% hold on 
% boxplot(step_charac_SettlingMax,mutant_templates)
% title('Boxplot of step charac SettlingMax')
% ylabel('SettlingMax')
% xlabel('Mutant templates')
% hold off
% 
% figure;
% hold on 
% boxplot(step_charac_Overshoot,mutant_templates)
% title('Boxplot of step charac Overshoot')
% ylabel('Overshoot')
% xlabel('Mutant templates')
% hold off
% 
% figure;
% hold on 
% boxplot(step_charac_Undershoot,mutant_templates)
% title('Boxplot of step charac Undershoot')
% ylabel('Undershoot')
% xlabel('Mutant templates')
% hold off
% 
% figure;
% hold on 
% boxplot(step_charac_Peak,mutant_templates)
% title('Boxplot of step charac Peak')
% ylabel('Peak')
% xlabel('Mutant templates')
% hold off
% 
% figure;
% hold on 
% boxplot(step_charac_PeakTime,mutant_templates)
% title('Boxplot of step charac PeakTime')
% ylabel('PeakTime')
% xlabel('Mutant templates')
% hold off
% 
% figure;
% hold on 
% boxplot(step_charac_RiseTime,mutant_templates)
% title('Boxplot of step charac RiseTime')
% ylabel('RiseTime')
% xlabel('Mutant templates')
% hold off


%%%%%%%%%% START COMMENT for figures %%%%%%%%%%%%%%%
% fig_RiseTime=figure(1);
% if ~exist('ExecueONce_1','var')
%     set(fig_RiseTime,'Name','RiseTime parameter','NumberTitle','off')
%     ExecueONce_1=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(1,:)','-*b',xin',dead_mutants_param(1,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% %set(h,'xTick',xin,'xticklabel',{'00','02','03','04','05','08','10','11','14','30','32','33'})
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_SettlingTime=figure(2);
% if ~exist('ExecueONce_2','var')
%     set(fig_SettlingTime,'Name','SettlingTime parameter','NumberTitle','off')
%     ExecueONce_2=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(2,:)','-*b',xin',dead_mutants_param(2,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_SettlingMin=figure(3);
% if ~exist('ExecueONce_3','var')
%     set(fig_SettlingMin,'Name','SettlingMin parameter','NumberTitle','off')
%     ExecueONce_3=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(3,:)','-*b',xin',dead_mutants_param(3,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_SettlingMax=figure(4);
% if ~exist('ExecueONce_4','var')
%     set(fig_SettlingMax,'Name','SettlingMax parameter','NumberTitle','off')
%     ExecueONce_4=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(4,:)','-*b',xin',dead_mutants_param(4,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_Overshoot=figure(5);
% if ~exist('ExecueONce_5','var')
%     set(fig_Overshoot,'Name','Overshoot parameter','NumberTitle','off')
%     ExecueONce_5=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(5,:)','-*b',xin',dead_mutants_param(5,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% 
% 
% fig_Undershoot=figure(6);
% if ~exist('ExecueONce_6','var')
%     set(fig_Undershoot,'Name','Undershoot parameter','NumberTitle','off')
%     ExecueONce_6=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(6,:)','-*b',xin',dead_mutants_param(6,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_Peak=figure(7);
% if ~exist('ExecueONce_7','var')
%     set(fig_Peak,'Name','Peak parameter','NumberTitle','off')
%     ExecueONce_7=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(7,:)','-*b',xin',dead_mutants_param(7,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_PeakTime=figure(8);
% if ~exist('ExecueONce_8','var')
%     set(fig_PeakTime,'Name','PeakTime parameter','NumberTitle','off')
%     ExecueONce_8=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(8,:)','-*b',xin',dead_mutants_param(8,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% 
% 
% % if ~exist('ExecueONce_1','var')
% %     [ax4,h3]=suplabel('First parameter criteria','t');
% %     set(h3,'FontSize',30);
% %     figure('Name','First parameter','NumberTitle','off')
% %     ExecueONce_1=0;
% % end
% 
% % dead_mutants_param(1,x_index)
% % live_mutants_param(1,x_index)
% 
% fig_t=figure(9);
% if ~exist('ExecueONce_t','var')
%     set(fig_t,'Name','All parameter','NumberTitle','off')
%     ExecueONce_t=0;
% end
% h = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants','-*b',xin',dead_mutants','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% fprintf('%d  \t %d  \t %d  \n',threshold_value, sum(live_mutants),sum(dead_mutants));
% title(h,tit);
% ylabel(h,'# mutants');
% xlabel(h,'Mutant templates');
% xTick=get(h,'xtick');
% %set(h,'xTick',xin,'xticklabel',{'00','02','03','04','05','08','10','11','14','30','32','33'})
% set(h,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
%%%%%%%%%% END COMMENT for figures %%%%%%%%%%%%%%%

%Print in console the output of # live and # dead mutants
%fprintf('%d  \t %d  \t %d  \n',threshold_value, sum(live_mutants),sum(dead_mutants));
%xdatalive(no_of_step_quantities_above_threshold, threshold_value+1)=sum(live_mutants);
%xdatadead(no_of_step_quantities_above_threshold, threshold_value+1)=sum(dead_mutants);
xdatalive(xaxisdata, yaxisdata)=sum(live_mutants);
xdatadead(xaxisdata, yaxisdata)=sum(dead_mutants);
yaxisdata=yaxisdata+1;


% if ~exist('ExecueONce_t','var')
%     [ax4,h3]=suplabel('All parameter criteria','t');
%     set(h3,'FontSize',30);
%     figure('Name','All parameter','NumberTitle','off')
%     ExecueONce_t=0;
% end


fig_index = fig_index+1;
%total_mutants % print for debug

end
xaxisdata=xaxisdata+1;
yaxisdata=1;
xdatalive
xdatadead
end

overalltotalmutants=820;
xvalues = {
    '1'  ,  '2' ,   '3'  ,  '4'  ,  '5'  ,  '6'  ,  '7',  '8'};
yvalues = {
    '0'  ,  '10'  ,  '20'  ,  '30'  ,  '40'  ,  '50'  ,  '60'  ,  '70'  ,  '80'  ,  '90'  ,  '100'};


% Create figure
figure1 = figure('Name','Boeing 747 airspeed % Live mutants');

% Create heatmap
heatmap(figure1,xvalues,yvalues,(xdatalive'./overalltotalmutants)*100,...
    'Colormap',[1 1 1;0.991596639156342 0.994771242141724 0.98711484670639;0.983193278312683 0.989542484283447 0.974229693412781;0.974789917469025 0.984313726425171 0.961344540119171;0.966386556625366 0.979084968566895 0.948459386825562;0.957983195781708 0.973856210708618 0.935574233531952;0.949579834938049 0.968627452850342 0.922689080238342;0.941176474094391 0.963398694992065 0.909803926944733;0.932773113250732 0.958169937133789 0.896918773651123;0.924369752407074 0.952941179275513 0.884033620357513;0.915966391563416 0.947712421417236 0.871148467063904;0.907563030719757 0.94248366355896 0.858263313770294;0.899159669876099 0.937254905700684 0.845378160476685;0.89075630903244 0.932026147842407 0.832493007183075;0.882352948188782 0.926797389984131 0.819607853889465;0.873949587345123 0.921568632125854 0.806722700595856;0.865546226501465 0.916339874267578 0.793837547302246;0.857142865657806 0.911111116409302 0.780952394008636;0.848739504814148 0.905882358551025 0.768067240715027;0.84033614397049 0.900653600692749 0.755182087421417;0.831932783126831 0.895424842834473 0.742296934127808;0.823529422283173 0.890196084976196 0.729411780834198;0.815126061439514 0.88496732711792 0.716526627540588;0.806722700595856 0.879738569259644 0.703641474246979;0.798319339752197 0.874509811401367 0.690756320953369;0.789915978908539 0.869281053543091 0.67787116765976;0.78151261806488 0.864052295684814 0.66498601436615;0.773109257221222 0.858823537826538 0.65210086107254;0.764705896377563 0.853594779968262 0.639215707778931;0.756302535533905 0.848366022109985 0.626330554485321;0.747899174690247 0.843137264251709 0.613445401191711;0.739495813846588 0.837908506393433 0.600560247898102;0.73109245300293 0.832679748535156 0.587675094604492;0.722689092159271 0.82745099067688 0.574789941310883;0.714285731315613 0.822222232818604 0.561904788017273;0.705882370471954 0.816993474960327 0.549019634723663;0.697479009628296 0.811764717102051 0.536134481430054;0.689075648784637 0.806535959243774 0.523249328136444;0.680672287940979 0.801307201385498 0.510364174842834;0.672268927097321 0.796078443527222 0.497478991746902;0.663865566253662 0.790849685668945 0.484593838453293;0.655462205410004 0.785620927810669 0.471708685159683;0.647058844566345 0.780392169952393 0.458823531866074;0.638655483722687 0.775163412094116 0.445938378572464;0.630252122879028 0.76993465423584 0.433053225278854;0.62184876203537 0.764705896377563 0.420168071985245;0.613445401191711 0.759477138519287 0.407282918691635;0.605042040348053 0.754248380661011 0.394397765398026;0.596638679504395 0.749019622802734 0.381512612104416;0.588235318660736 0.743790864944458 0.368627458810806;0.579831957817078 0.738562107086182 0.355742305517197;0.571428596973419 0.733333349227905 0.342857152223587;0.563025236129761 0.728104591369629 0.329971998929977;0.554621875286102 0.722875833511353 0.317086845636368;0.546218514442444 0.717647075653076 0.304201692342758;0.537815153598785 0.7124183177948 0.291316539049149;0.529411792755127 0.707189559936523 0.278431385755539;0.521008431911469 0.701960802078247 0.265546232461929;0.51260507106781 0.696732044219971 0.25266107916832;0.504201710224152 0.691503286361694 0.239775910973549;0.495798319578171 0.686274528503418 0.226890757679939;0.487394958734512 0.681045770645142 0.21400560438633;0.478991597890854 0.675817012786865 0.20112045109272;0.470588237047195 0.670588254928589 0.18823529779911],...
    'FontSize',15,...
    'YLabel','% of allowed absolute error',...
    'XLabel','# step quantities');


% Create figure
figure2 = figure('Name','Boeing 747 airspeed % Dead mutants');

% Create heatmap
heatmap(figure2,xvalues,yvalues,(xdatadead'./overalltotalmutants)*100,...
    'Colormap',[1 1 1;0.994273245334625 0.985371947288513 0.986990332603455;0.988546550273895 0.970743834972382 0.973980724811554;0.982819795608521 0.956115782260895 0.960971057415009;0.977093040943146 0.941487729549408 0.947961390018463;0.971366345882416 0.926859617233276 0.934951782226562;0.965639591217041 0.91223156452179 0.921942114830017;0.959912836551666 0.897603511810303 0.908932447433472;0.954186141490936 0.882975399494171 0.895922839641571;0.948459386825562 0.868347346782684 0.882913172245026;0.942732632160187 0.853719294071198 0.86990350484848;0.937005937099457 0.839091181755066 0.85689389705658;0.931279182434082 0.824463129043579 0.843884229660034;0.925552427768707 0.809835016727448 0.830874562263489;0.919825732707977 0.795206964015961 0.817864894866943;0.914098978042603 0.780578911304474 0.804855287075043;0.908372223377228 0.765950798988342 0.791845619678497;0.902645528316498 0.751322746276855 0.778835952281952;0.896918773651123 0.736694693565369 0.765826344490051;0.891192018985748 0.722066581249237 0.752816677093506;0.885465323925018 0.70743852853775 0.73980700969696;0.879738569259644 0.692810475826263 0.72679740190506;0.874011814594269 0.678182363510132 0.713787734508514;0.868285119533539 0.663554310798645 0.700778067111969;0.862558364868164 0.648926258087158 0.687768459320068;0.856831610202789 0.634298145771027 0.674758791923523;0.851104915142059 0.61967009305954 0.661749124526978;0.845378160476685 0.605042040348053 0.648739516735077;0.83965140581131 0.590413928031921 0.635729849338531;0.83392471075058 0.575785875320435 0.622720181941986;0.828197956085205 0.561157822608948 0.609710574150085;0.82247120141983 0.546529710292816 0.59670090675354;0.8167445063591 0.531901657581329 0.583691239356995;0.811017751693726 0.517273604869843 0.570681631565094;0.805290997028351 0.502645492553711 0.557671964168549;0.799564301967621 0.488017439842224 0.544662296772003;0.793837547302246 0.473389357328415 0.531652688980103;0.788110792636871 0.458761274814606 0.518643021583557;0.782384097576141 0.444133222103119 0.505633354187012;0.776657342910767 0.42950513958931 0.492623716592789;0.770930588245392 0.4148770570755 0.479614078998566;0.765203893184662 0.400249004364014 0.46660441160202;0.759477138519287 0.385620921850204 0.453594774007797;0.753750383853912 0.370992839336395 0.440585136413574;0.748023688793182 0.356364756822586 0.427575469017029;0.742296934127808 0.341736704111099 0.414565831422806;0.736570179462433 0.32710862159729 0.401556193828583;0.730843484401703 0.312480539083481 0.388546526432037;0.725116729736328 0.297852486371994 0.375536888837814;0.719389975070953 0.283224403858185 0.362527251243591;0.713663280010223 0.268596321344376 0.349517583847046;0.707936525344849 0.253968268632889 0.336507946252823;0.702209770679474 0.23934018611908 0.323498278856277;0.696483075618744 0.22471210360527 0.310488641262054;0.690756320953369 0.210084035992622 0.297479003667831;0.685029566287994 0.195455968379974 0.284469336271286;0.679302871227264 0.180827885866165 0.271459698677063;0.67357611656189 0.166199818253517 0.25845006108284;0.667849361896515 0.151571735739708 0.245440408587456;0.662122666835785 0.13694366812706 0.232430756092072;0.65639591217041 0.122315593063831 0.219421103596687;0.650669157505035 0.107687525451183 0.206411451101303;0.644942462444305 0.0930594503879547 0.19340181350708;0.639215707778931 0.0784313753247261 0.180392161011696],...
    'FontSize',15,...
    'YLabel','% of allowed absolute error',...
    'XLabel','# step quantities');



% mut_folder='/home/balaji/clang-llvm/mutationdocker/code/mutants_b747cl';
% % mut_folder='/home/balaji/clang-llvm/cppdocker/code/mutator/02';
% cd(mut_folder)
% listoffiles = dir('*.c');
% C_nooffiles = numel(listoffiles);
% listoffiles = dir('*.txt');
% TXT_nooffiles = numel(listoffiles);
% listoffiles = dir('*.*');
% ALL_nooffiles = numel(listoffiles);
% 
% nooffiles=C_nooffiles;
% nooffiles
% 
% %make sure we are at the right place before the simulation starts
% home = '/home/balaji/mutation/airlib/';
% cd(home)
% 
% 
% format long;
% %load base values to compare with
% load('/home/balaji/clang-llvm/cppdocker/code/mutator/b747cl_original.mat')
% base_value = rt_airspeed;
% 
% 
% running_mutants_per_order = [];
% blowing_mutants_per_order = [];
% 
% step_charac_RiseTime = [];
% step_charac_SettlingTime = [];
% step_charac_SettlingMin = [];
% step_charac_SettlingMax = [];
% step_charac_Overshoot = [];
% step_charac_Undershoot = [];
% step_charac_Peak = [];
% step_charac_PeakTime = [];
% 
% %four characterization models for airspeed
% airspeed_error_m1 = [];
% airspeed_error_m2 = [];
% airspeed_error_m3 = [];
% airspeed_error_m4 = [];
% 
% %four characterization models for airspeed for non blowing models only
% airspeed_error_non_blow_m1 = [];
% airspeed_error_non_blow_m2 = [];
% airspeed_error_non_blow_m3 = [];
% airspeed_error_non_blow_m4 = [];
% 
% %for order 1 to 10
% for order = 1:1
%     
%     error_order = 0;
%     blow_mutant_count = 0;
%     running_mutant_count = 0;
%     
%     %four characterization models for airspeed
%     airspeed_error_m1_order = 0;
%     airspeed_error_m2_order = 0;
%     airspeed_error_m3_order = 0;
%     airspeed_error_m4_order = 0;
% 
%     %four characterization models for airspeed for non blowing models only
%     airspeed_error_non_blow_m1_order = 0;
%     airspeed_error_non_blow_m2_order = 0;
%     airspeed_error_non_blow_m3_order = 0;
%     airspeed_error_non_blow_m4_order = 0;
%     
%     %for mutant 1 to 200
%     for i = 1:nooffiles
%                
%         %Generate right file paths
%         %file_path="/home/balaji/clang-llvm/mutationdocker/code/mutants_b747cl";
%         %file_path = ['/home/balaji/mutation/airlib/Mutants/order_' int2str(order) '/mutants/' int2str(i)];
%         file_path = mut_folder;
%         fid = fopen([file_path '/log2_' int2str(i) '.txt'],'r');
%         if fid == -1
%             error('Author:Function:OpenFile', 'Cannot open file: %s', [file_path '/log2_' int2str(i) '.txt']);
%         end
%         log= textscan(fid,'%s'); 
%         %if the mutant failed execution/or was not compiled successfully
%         if (strcmp(log{1}, 'NO')) 
%             step_charac_RiseTime(i)= -1000;
%             step_charac_SettlingTime(i)= -1000;
%             step_charac_SettlingMin(i)= -1000;
%             step_charac_SettlingMax(i)= -1000;
%             step_charac_Overshoot(i)= -1000;
%             step_charac_Undershoot(i)= -1000;
%             step_charac_Peak(i)= -1000;
%             step_charac_PeakTime(i)= -1000;
%             continue 
%         end
%         fclose(fid);
%         
%         file = [file_path '/b747cl_' int2str(i) '.mat'];
%         load(file)
%         sti = stepinfo(rt_airspeed,rt_t,151.148);
%         
%         %increase mutant count
%         running_mutant_count = running_mutant_count + 1;
%         
%         is_blow_up = false;
%         mutant_value = rt_airspeed;
%         error = 0;
%         model = 0;
%         max_error = 0;
%         
%         loop_size = size(mutant_value);
%         %over time
%         for j = 1:loop_size(1)
%             
%             if(isnan(mutant_value(j)) || mutant_value(j) == Inf || mutant_value(j) == -Inf)
%                 %system blowed up for sure: easy to detect
%                 is_blow_up = true;
%             else
%                 i_error = (base_value(j) - mutant_value(j));
%                 error = error + i_error; 
%                 if(max_error < abs(i_error))
%                     max_error = abs(i_error);
%                 end
%             end
%         end
%         
%         step_charac_RiseTime(i)= sti.RiseTime;
%         step_charac_SettlingTime(i)= sti.SettlingTime;
%         step_charac_SettlingMin(i)= sti.SettlingMin;
%         step_charac_SettlingMax(i)= sti.SettlingMax;
%         step_charac_Overshoot(i)= sti.Overshoot;
%         step_charac_Undershoot(i)= sti.Undershoot;
%         step_charac_Peak(i)= sti.Peak;
%         step_charac_PeakTime(i)= sti.PeakTime;
%         
%         
%         if(abs(i_error) > 100)
%             is_blow_up = true;
%         end
%         
%        %populate error for all models for every mutant
%         airspeed_error_m1_order = airspeed_error_m1_order + error;
%         airspeed_error_m2_order = airspeed_error_m2_order + ((error / loop_size(1)) * (error/loop_size(1)));
%         airspeed_error_m3_order = airspeed_error_m3_order + abs(error);
%         airspeed_error_m4_order = airspeed_error_m4_order + max_error;
%         
%         %populate error for all models for every non blowing mutant 
%         if(~is_blow_up)
%             airspeed_error_non_blow_m1_order = airspeed_error_non_blow_m1_order + error;
%             airspeed_error_non_blow_m2_order = airspeed_error_non_blow_m2_order + ((error / loop_size(1)) * (error/loop_size(1)));
%             airspeed_error_non_blow_m3_order = airspeed_error_non_blow_m3_order + abs(error);
%             airspeed_error_non_blow_m4_order = airspeed_error_non_blow_m4_order + max_error;
%         else
%             blow_mutant_count = blow_mutant_count +1;
%         end
%         
%     end
%     
%     %four characterization models for airspeed
%     airspeed_error_m1 = [airspeed_error_m1 (airspeed_error_m1_order / running_mutant_count)];
%     airspeed_error_m2 = [airspeed_error_m2 (airspeed_error_m2_order / running_mutant_count)];
%     airspeed_error_m3 = [airspeed_error_m3 (airspeed_error_m3_order / running_mutant_count)];
%     airspeed_error_m4 = [airspeed_error_m4 (airspeed_error_m4_order / running_mutant_count)];
% 
%     %four characterization models for airspeed for non blowing models only
%     airspeed_error_non_blow_m1 = [airspeed_error_non_blow_m1 (airspeed_error_non_blow_m1_order / (running_mutant_count - blow_mutant_count))];
%     airspeed_error_non_blow_m2 = [airspeed_error_non_blow_m2 (airspeed_error_non_blow_m2_order / (running_mutant_count - blow_mutant_count))];
%     airspeed_error_non_blow_m3 = [airspeed_error_non_blow_m3 (airspeed_error_non_blow_m3_order / (running_mutant_count - blow_mutant_count))];
%     airspeed_error_non_blow_m4 = [airspeed_error_non_blow_m4 (airspeed_error_non_blow_m4_order / (running_mutant_count - blow_mutant_count))];
%     
%     running_mutants_per_order = [running_mutants_per_order running_mutant_count];
%     blowing_mutants_per_order = [blowing_mutants_per_order blow_mutant_count];
% end
% 
% 
% %load base values to compare with
% load('/home/balaji/clang-llvm/cppdocker/code/mutator/b747cl_original.mat')
% base_value= rt_airspeed;
% sti = stepinfo(rt_airspeed,rt_t,151.148);
% 
% x=1:1:nooffiles;
% %plot(x,step_charac_RiseTime,x,step_charac_SettlingTime,x,step_charac_SettlingMin,x,step_charac_SettlingMax,x,step_charac_Overshoot,x,step_charac_Undershoot,x,step_charac_Peak,x,step_charac_PeakTime);
% 
% 
% fig = figure;
% hold on 
% plot(x,step_charac_RiseTime,'bx',x,sti.RiseTime,'ro')
% title('step charac RiseTime')
% hold off
% 
% fig = figure;
% hold on 
% plot(x,step_charac_SettlingTime,'bx',x,sti.SettlingTime,'ro')
% title('step charac SettlingTime')
% hold off
% 
% fig = figure;
% hold on 
% plot(x,step_charac_SettlingMin,'bx',x,sti.RiseTime,'ro')
% title('step charac SettlingMin')
% hold off
% 
% fig = figure;
% hold on 
% plot(x,step_charac_SettlingMax,'bx',x,sti.SettlingMax,'ro')
% title('step charac SettlingMax')
% hold off
% 
% fig = figure;
% hold on 
% plot(x,step_charac_Overshoot,'bx',x,sti.Overshoot,'ro')
% title('step charac Overshoot')
% hold off
% 
% fig = figure;
% hold on 
% plot(x,step_charac_Undershoot,'bx',x,sti.Undershoot,'ro')
% title('step charac Undershoot')
% hold off
% 
% fig = figure;
% hold on 
% plot(x,step_charac_Peak,'bx',x,sti.Peak,'ro')
% title('step charac Peak')
% hold off
% 
% fig = figure;
% hold on 
% plot(x,step_charac_PeakTime,'bx',x,sti.PeakTime,'ro')
% title('step charac PeakTime')
% hold off


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% already commented
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% %Print the Blow off details about Mutants
%     fig = figure;
%     hold on 
%     plot(running_mutants_per_order,'--b*','MarkerSize',10,'LineWidth',2)
%     plot(blowing_mutants_per_order,'--r*','MarkerSize',10,'LineWidth',2)
%     plot(running_mutants_per_order - blowing_mutants_per_order,'--g*','MarkerSize',10,'LineWidth',2)
%     axis([1 10 0 200 ])
%     legend('ALL Mutants','Dead Mutants', 'Alive Mutants')
%     xlabel('Order of Mutants','FontSize',12,'FontWeight','bold') % x-axis label
%     ylabel('Dead / Alive Mutants for Variable Airspeed','FontSize',12,'FontWeight','bold') % y-axis label
%     print(fig,'figs/Airspeed/MutantsAirspeed','-dpng');
%     hold off 
%     close(fig)
% 
% %Print the Blow off details about Mutants (Percentage)
% 
% blow_percentage = bsxfun(@rdivide,(blowing_mutants_per_order * 100),running_mutants_per_order);
% alive_percentage = bsxfun(@rdivide,((running_mutants_per_order - blowing_mutants_per_order)*100), running_mutants_per_order);
% fig = figure;
% hold on 
% plot(blow_percentage,'--r*','MarkerSize',10,'LineWidth',2)
% plot(alive_percentage ,'--g*','MarkerSize',10,'LineWidth',2)
% axis([1 10 0 100 ])
% legend('Dead Mutants', 'Alive Mutants')
% xlabel('Order of Mutants','FontSize',12,'FontWeight','bold') % x-axis label
% ylabel('% Dead / Alive Mutants for Variable Airspeed','FontSize',12,'FontWeight','bold') % y-axis label
% print(fig,'figs/Airspeed/MutantsAirspeedPercentage','-dpng');
% hold off 
% close(fig)
% 
% 
% 
% 
% %Print the model 1
% fig = figure;
% hold on 
% plot(airspeed_error_m1,'--b*','MarkerSize',10,'LineWidth',2)
% xlabel('Order of Mutants','FontSize',12,'FontWeight','bold') % x-axis label
% ylabel('Sum of error for all Mutants(Airspeed)','FontSize',12,'FontWeight','bold') % y-axis label
% print(fig,'figs/Airspeed/model1','-dpng');
% hold off 
% close(fig)
% 
% %Print the model 1 for alive only
% fig = figure;
% hold on 
% plot(airspeed_error_non_blow_m1,'--b*','MarkerSize',10,'LineWidth',2)
% xlabel('Order of Mutants','FontSize',12,'FontWeight','bold') % x-axis label
% ylabel('Sum of error for alive Mutants(Airspeed)','FontSize',12,'FontWeight','bold') % y-axis label
% print(fig,'figs/Airspeed/model1_alive','-dpng');
% hold off 
% close(fig)
% 
% 
% 
% %Print the model 2
% fig = figure;
% hold on 
% plot(airspeed_error_m2,'--b*','MarkerSize',10,'LineWidth',2)
% legend('Every value is inf')
% xlabel('Order of Mutants','FontSize',12,'FontWeight','bold') % x-axis label
% ylabel('Mean squared error for all Mutants(Airspeed)','FontSize',12,'FontWeight','bold') % y-axis label
% print(fig,'figs/Airspeed/model2','-dpng');
% hold off 
% close(fig)
% 
% %Print the model 2 for alive only
% fig = figure;
% hold on 
% plot(airspeed_error_non_blow_m2,'--b*','MarkerSize',10,'LineWidth',2)
% xlabel('Order of Mutants','FontSize',12,'FontWeight','bold') % x-axis label
% ylabel('Mean Squared error for alive Mutants(Airspeed)','FontSize',12,'FontWeight','bold') % y-axis label
% print(fig,'figs/Airspeed/model2_alive','-dpng');
% hold off 
% close(fig)
% 
% 
% 
% 
% %Print the model 3
% fig = figure;
% hold on 
% plot(airspeed_error_m3,'--b*','MarkerSize',10,'LineWidth',2)
% xlabel('Order of Mutants','FontSize',12,'FontWeight','bold') % x-axis label
% ylabel('Sum of absolute error for all Mutants(Airspeed)','FontSize',12,'FontWeight','bold') % y-axis label
% print(fig,'figs/Airspeed/model3','-dpng');
% hold off 
% close(fig)
% 
% %Print the model 3 for alive only
% fig = figure;
% hold on 
% plot(airspeed_error_non_blow_m3,'--b*','MarkerSize',10,'LineWidth',2)
% xlabel('Order of Mutants','FontSize',12,'FontWeight','bold') % x-axis label
% ylabel('Sum of absolute error for alive Mutants(Airspeed)','FontSize',12,'FontWeight','bold') % y-axis label
% print(fig,'figs/Airspeed/model3_alive','-dpng');
% hold off 
% close(fig)
% 
% 
% 
% 
% %Print the model 4
% fig = figure;
% hold on 
% plot(airspeed_error_m4,'--b*','MarkerSize',10,'LineWidth',2)
% xlabel('Order of Mutants','FontSize',12,'FontWeight','bold') % x-axis label
% ylabel('Max error for alive Mutants(Airspeed)','FontSize',12,'FontWeight','bold') % y-axis label
% print(fig,'figs/Airspeed/model4','-dpng');
% hold off 
% close(fig)
% 
% %Print the model 4 for alive only
% fig = figure;
% hold on 
% plot(airspeed_error_non_blow_m4,'--b*','MarkerSize',10,'LineWidth',2)
% xlabel('Order of Mutants','FontSize',12,'FontWeight','bold') % x-axis label
% ylabel('Max error for alive Mutants(Airspeed)','FontSize',12,'FontWeight','bold') % y-axis label
% print(fig,'figs/Airspeed/model4_alive','-dpng');
% hold off 
% close(fig)



fclose('all')