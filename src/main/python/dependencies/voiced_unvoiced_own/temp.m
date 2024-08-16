function [vuv3,vuv2] = voiced_unvoiced_own(x1,fs)
y1=abs(x1);
%%
w1=gausswin(20*fs/1000,4);
y2=filtfilt(w1,1,y1);
y2=y2/max(y2); y2(1)=0; y2(end)=0;
Th=1/25;
vuv=y2-y2;
vuv(y2>Th)=1;
% [vnvsig,vnvevi,zf,gci,es,f0] = VNVSig(x1,fs);
pind=find(diff(vuv)==1);
nind=find(diff(vuv)==-1);
%% Merging the boundaries
vuv2=vuv;
for i=1:length(pind)-1
    
    D=pind(i+1)-nind(i);
    if(D<40*fs/1000)
        vuv2(nind(i):pind(i+1))=1;
    end
end
%% remvoing the false boundaries  
vuv3=vuv2;
pind=find(diff(vuv2)==1);
nind=find(diff(vuv2)==-1);
for i=1:length(pind)
    D=nind(i)-pind(i);
    if(D<30*fs/1000)
        vuv3(pind(i):nind(i))=0;
    end 
end
pind=find(diff(vuv3)==1);
nind=find(diff(vuv3)==-1);
if(length(pind)==1)
    wave=x1;
   
else
 for i=1:length(pind)
   
     if(i==1)
        i
        reg1=x1(1:floor((nind(i)+pind(i+1))/2));

        t=1:floor((nind(i)+pind(i+1))/2);
    temppind=1;
    else

        if(i<length(pind))
            i
            reg1=x1(floor((nind(i-1)+pind(i))/2)+1:floor((nind(i)+pind(i+1))/2));

            t=floor((nind(i-1)+pind(i))/2)+1:floor((nind(i)+pind(i+1))/2);
        temppind=floor((nind(i-1)+pind(i))/2);
        else

            reg1=x1(floor((nind(i-1)+pind(i))/2)+1:end);

            t=floor((nind(i-1)+pind(i))/2)+1:length(x1);
        temppind=floor((nind(i-1)+pind(i))/2);
        end

    end
    
    wave=reg1;

 end 
 
end
% t1=[1:length(x1)]/fs;
% p1=subplot(2,1,1);
% % xlim([0.2 4])
% plot(t1,x1)
% % % xlim([0.2 4])
% axis tight;
% p2=subplot(212);
% plot(t1,x1)
% hold on;
% plot(t1,vuv3*0.65)
% % % xlim([0.2 4])
% axis tight;
% % p3=subplot(4,1,3);
% plot(t1,x1)
% hold on;
% plot(t1,vuv2*0.95)
% % xlim([0.2 4])
% axis tight;
% p4=subplot(4,1,4);
% plot(t1,x1)
% hold on;
% plot(t1,vuv3*0.95)
% % xlim([0.2 4])
% axis tight;
% linkaxes([p1,p2],'x')
end
