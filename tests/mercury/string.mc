
extern proc malloc(sz usize) ^u8;
extern proc free(ptr ^u8);

export
struct String {
    buffer  ^char;
    count    usize;
    capacity usize;
}

impl String {
    export proc init() Self {
	var str Self = zeroed;
	return str;
    }

    export proc reserve(&self, cap usize) {
	if cap <= self.capacity {
	    return;
	}
	const new_cap = cap;
	var new_buffer ^char = malloc(new_cap) as ^char;
	foreach n; 0..self.count {
	    new_buffer[n] = self.buffer[n];
	}
	free(self.buffer as ^u8);
	self.buffer = new_buffer;
	self.capacity = new_cap;
    }
    
    proc append_char(&self, ch char) {
	if self.count >= self.capacity {
	    var new_cap = self.capacity * 2;
	    if new_cap == 0 {
		new_cap = 10;
	    }
	    var new_buffer ^char = malloc(new_cap) as ^char;
	    foreach n; 0..self.count {
		new_buffer[n] = self.buffer[n];
	    }
	    free(self.buffer as ^u8);
	    self.buffer = new_buffer;
	    self.capacity = new_cap;
	}
    }
}
