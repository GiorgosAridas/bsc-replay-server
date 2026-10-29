def compute_match_clock(milliseconds, period):
    seconds = milliseconds // 1000
    minutes = seconds // 60
    if period == 2:
        minutes = minutes + 45
    seconds = seconds % 60
    clock = f"{minutes:02d}:{seconds:02d}"
    return clock



def compute_milliseconds(timestamp):     #we take a timestamp at the following form 00:12:50.372
    iterator = iter(timestamp)
    numberofiterations = 1
    for num in iterator:
        if (num == ":" or num == "."):     #we throw away the partw we don't need
            continue
        
        second = next(iterator, None)
        if numberofiterations == 1:         #At the 1st iteration we compute the hour 
            hour = int(num)*10 + int(second)
        elif numberofiterations == 2:
            minutes = int(num)*10 + int(second) #At the 2nd iteration we compute the hour
        else:
            seconds = int(num)*10000 + int(second)*1000  #we compute firstly the first part until the dot
            next(iterator)
            seconds = seconds + int(next(iterator)) * 100 + int(next(iterator)) * 10 + int(next(iterator))
        
        numberofiterations = numberofiterations + 1



    milliseconds = hour*60*60*1000 + minutes*60*1000 + seconds
    return milliseconds