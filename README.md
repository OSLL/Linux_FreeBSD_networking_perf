# Linux_FreeBSD_networking_perf

## Команды

### stats \<protocol\>
Выводит различную статистику указанного протокола

Поддерживаемые протоколы:

Linux: ip, tcp, udp, udplite, icmp, mptcp, ip6, udp6, udplite6, icmp6

FreeBSD: ip, tcp, udp, icmp, igmp

### iperf3 \<filename\>
Анализирует файл, полученный от iperf3 с флагом -J. Выводит временной интервал и число бит, переданных за это время

### list sockets \<protocol\> 
Показывает список активных сокетов, работающих по заданному протоколу

Поддерживаемые протоколы:

Linux: tcp, tcp6, udp, udp6, udplite, udplite6, raw, raw6, icmp, icmp6

FreeBSD: tcp, udp, raw

### measure 
#### rx-timings 
Измеряет время нахождения пакета в системе во время приема, получает пакеты от measure tx-timings

Выводит следующую информацию:

* Hardware - время от получения данных устройством до прибытия в приложение (требует поддержки от устройства и не доступно во FreeBSD)
* Software - время от получения данных ядром до прибытия в приложение (во FreeBSD доступно только для UDP)
* In Call - время, которое программа провела, находясь в вызовах recv
* Total - время от отправки данных с помощью measure tx-timings до получения приложением

#### tx-timings 
Отправляет пакеты measure rx-timings. 
Так же доступно измерение времени нахождения пакета в системе, выводится следующая информация:

* Hardware: время от отправки данных приложением до получения их устройством (требует поддержки от устройства, используется при measure-type = hardware)
* Software: время от отправки данных приложением до момента, когда информация покинет ядро (при measure-type = software), до момента попадания пакета в планировщик (при measure-type = scheduler), до момента, когда получение всей информации было подтверждено (реализовано только для tcp и tcp6, используется при measure-type = ack)
* In Call - время, которое программа провела, находясь в вызовах send

Для FreeBSD не доступно получение времени от сетевого устройства (hardware), Время от системы (software) доступно только для udp, поддерживается только measure-type = software, при других значениях measure-type игнорируется 
  
#### Аргументы

* protocol - используемый протокол. ***tcp***, udp, sctp, udplite, tcp6, udp6, udplite6 (Для Linux так же доступны: mptcp, mptcp6, sctp6)
* port - используемый порт для отправки/прослушки
* packets-count - число отправляемых/принимаемых пакетов. Время расчитывается как среднее арифметическое.
* address - ip-адрес, на который отправлять данные
* measure-type - вид измерения, описаны в tx-timings. ***software***, scheduler, hardware, ack
* ns - если установлен, информацию выводится в наносекундах (по умолчанию - микросекунды). Не имеет смысла во FreeBSD, 
так как software timestamp возвращается с точностью до микросекунды.
* data - имя файла, из которого будет браться информация для отправки даннных
* data-size - размер отправляемых данных. При использовании с data - обрезает файл, без указания - используется для создание случайных данных заданной длины
* zero-copy - если указан, то используется вызов sendfile, предотвращающий копирование данных файла из kernel-space в user-space

### get-cpu-distribution

Возвращает значения, которые можно интерпретировать как количество работы, совершенное каждым процессором по отдельности