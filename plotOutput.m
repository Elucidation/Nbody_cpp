clear all; close all; clc;
fclose('all');
MODE_REALTIME = 1;
MODE_PLOT = 2;
CHUNK_DEFAULT = 5; % Default chunk length, 1 means every step

%MODE = MODE_PLOT; % Doesn't work well with large files...
MODE = MODE_REALTIME;

DIMS = 3; % or 3


%%%%%%%%%%%%%%%%%%

%filename = 'testOutput.out';
filename = input('Enter filename: ','s');
if length(filename) < 4 || ~strcmp(filename(end-3:end),'.out')
    filename = [filename '.out'];
end
fprintf('Reading file %s...\n',filename);
f = fopen(filename,'r');
if (f == -1)
    disp('Couldn''t read input file')
    break
end

head = fgetl(f);

disp(head);
headdata = sscanf(head,'SIMULATING %d BODIES, %d STEPS, %g DT');
n = headdata(1);
steps = headdata(2);
dt = headdata(3);

% CHUNKING OF FILE
CHUNK_LENGTH = inf; % Chunks of file to read at a time (for large files)
if n*steps > 1e6
    disp('Data size too big, chunking file');
    CHUNK_LENGTH = CHUNK_DEFAULT; % Chunks of file to read at a time (for large files)
end



plotSlice = @(t,data) plot(data((t-1)*n+1:t*n,1),data((t-1)*n+1:t*n,2),'.k');
plotSlice3 = @(t,data) plot3(data((t-1)*n+1:t*n,1),data((t-1)*n+1:t*n,2),data((t-1)*n+1:t*n,3),'.k');

% GRAPHICS
colors = 'kbgry';
getColor = @(x) colors(mod(x,length(colors))+1);
figure;
axis equal;


chunk_count = 0;
while 1
    % x y z vx vy vz columns
    data = fscanf(f,'%f %f %f %f %f %f', [6 CHUNK_LENGTH*n])';
    if size(data,1) == 0
        disp('--Reached end of file--');
        break
    end
    fprintf('Reading Chunks %g to %g\n',chunk_count,chunk_count+CHUNK_LENGTH);
    if (MODE == MODE_PLOT)
        if DIMS == 2
            plot(data(1:n,1),data(1:n,2), 'og'  );
        elseif DIMS == 3
            plot3(data(1:n,1),data(1:n,2),data(1:n,3), 'og'  );
        end
        hold on;    
        for i = 1:n
            if DIMS == 2
                plot(data(i:n:end,1),data(i:n:end,2),  sprintf('.-%c',getColor(i))  );
            elseif DIMS == 3
                plot3(data(i:n:end,1),data(i:n:end,2),data(i:n:end,3),  sprintf('.-%c',getColor(i))  );
            end
            
        end
        hold off;
    elseif (MODE == MODE_REALTIME)
        for t = 1:(size(data,1)/n)
            if DIMS == 2
                plot(data((t-1)*n+1:t*n,1),data((t-1)*n+1:t*n,2),'.k');
            elseif DIMS == 3
                plot3(data((t-1)*n+1:t*n,1),data((t-1)*n+1:t*n,2),data((t-1)*n+1:t*n,3),'.k');
            end
            title(sprintf('%i bodies, %i steps, %g dt, Time: %g/%g',n,steps,dt,t*dt+dt*chunk_count,dt*steps));
            %hold off;
            %axis([-2 2 -2 2]);
            axis equal;
            pause(0.001);
            %pause
        end
    end
    %axis([-1 1 -1 1 -1 1]*10);
    chunk_count = chunk_count + CHUNK_LENGTH;
    pause(0.0001);
    %pause
end
fclose(f);