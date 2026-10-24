def compute_match_clock(milliseconds):
    seconds = milliseconds // 1000
    minutes = seconds // 60
    seconds = seconds % 60
    clock = f"{minutes:02d}:{seconds:02d}"
    return clock



def compute_milliseconds(timestamp):
    iterator = iter(timestamp)
    numberofiterations = 1
    for num in iterator:
        if (num == ":" or num == "."):
            continue
        
        second = next(iterator, None)
        if numberofiterations == 1:
            hour = int(num)*10 + int(second)
        elif numberofiterations == 2:
            minutes = int(num)*10 + int(second)
        else:
            seconds = int(num)*10000 + int(second)*1000
            next(iterator)
            seconds = seconds + int(next(iterator)) * 100 + int(next(iterator)) * 10 + int(next(iterator))
        
        numberofiterations = numberofiterations + 1



    milliseconds = hour*60*60*1000 + minutes*60*1000 + seconds
    return milliseconds