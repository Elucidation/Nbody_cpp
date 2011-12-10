clear all; close all; clc;
MODE_REALTIME = 1;
MODE_PLOT = 2;

%MODE = MODE_PLOT;
MODE = MODE_REALTIME;


%filename = 'testOutput.out';
filename = input('Enter filename: ','s');
fprintf('Reading file %s...\n',filename);

f = fopen(filename,'r');

head = fgetl(f);

disp(head);
headdata = sscanf(head,'SIMULATING %d BODIES, %d STEPS, %g DT');
n = headdata(1);
steps = headdata(2);
dt = headdata(3);


% x y z vx vy vz columns
data = fscanf(f,'%f %f %f %f %f %f', [6 inf])';
fclose(f);

plotSlice = @(t,data) plot(data((t-1)*n+1:t*n,1),data((t-1)*n+1:t*n,2),'.k');


% GRAPHICS
colors = 'kbgry';
getColor = @(x) colors(mod(x,length(colors))+1);
figure;
axis equal;
if (MODE == MODE_PLOT)
    hold on;    
    for i = 1:n
        plot(data(i:n:end,1),data(i:n:end,2),  sprintf('.-%c',getColor(i))  );
    end
    hold off;
elseif (MODE == MODE_REALTIME)
    for t = 1:(size(data,1)/n)
        %for i = 1:n
        %    plot(data((t-1)*n + i,1),data((t-1)*n + i,2),  sprintf('.%c',getColor(i))  );
        %    hold on;
        %end
        plot(data((t-1)*n+1:t*n,1),data((t-1)*n+1:t*n,2),'.k');
        title(sprintf('%i bodies, %i steps, %g dt, Time: %g/%g',n,steps,dt,t*dt,dt*steps));
        %hold off;
        %axis([-2 2 -2 2]);
        %axis([-1 1 -1 1]*1e2);
        axis equal;
        pause(0.001);
    end
end