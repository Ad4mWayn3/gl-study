const std = @import("std");
const rl = @import("raylib");
const rgui = @import("raygui");
const math = rl.math;
const allocator = std.heap.page_allocator;

const res = .{ .w = 1920, .h = 1080 };

const Game = struct {
	map: std.ArrayList(rl.Rectangle),
	selectorTip: rl.Vector2,
	selectorTail: rl.Vector2,
	camera: rl.Camera2D,
	mode: enum { scroll, build, edit },
	const Seconds = f32;
	const Self = @This();
	const kInputs = [_]rl.KeyboardKey{ .e, .s, .d, .f };

	fn init(self: *Self, gpa: std.mem.Allocator) !void {
		self.map = try .initCapacity(gpa, 0x40);
		self.mode = .scroll;
		self.camera = .{ 
			.offset = .{ .x = 0, .y = 0 },
			.rotation = 0,
			.target = .{ .x = 0, .y = 0 },
			.zoom = 1
		};
		try loadRectsFromFile("map.bin", gpa, &self.map);
	}

	fn deinit(self: *Self, gpa: std.mem.Allocator) void {
		self.map.deinit(gpa);
	}

	fn update(self: *Self, gpa: std.mem.Allocator, delta: Seconds) !void {
		const mousePos: rl.Vector2 =
			rl.getMousePosition().add(self.camera.target);

		inline for (kInputs) |kInput|
			if (rl.isKeyDown(kInput)) self.handleInput(kInput, delta);
		if (rl.isMouseButtonDown(.right)) {
			self.camera.target = self.camera.target.subtract(rl.getMouseDelta());
		}
		if (rl.isMouseButtonPressed(.left)) {
			self.selectorTip = mousePos;
			self.selectorTail = mousePos;
		} else if (rl.isMouseButtonDown(.left)) {
			self.selectorTail = mousePos;
		} else if (rl.isMouseButtonReleased(.left)
			and self.selectorTail.equals(self.selectorTip)==0) {
			try self.map.append(gpa, 
				rectangleTipTail(self.selectorTip, self.selectorTail));
		} else {
			self.selectorTip = self.selectorTail;
		}

		if (rgui.button(.{.x=40, .y=40, .width=200, .height=60}, "build")) {
			self.mode = .build;
		} if (rgui.button(.{.x=40, .y=120, .width=200, .height=60}, "edit")) {
			self.mode = .edit;
		}
	}

	fn draw(self: *Self) void {
		rl.beginMode2D(self.camera);
		defer rl.endMode2D();
		for (self.map.items) |rec| rl.drawRectangleRec(rec, .blue);
		//rl.drawRectangleV(self.selectorTip, 
		 	//self.selectorTail.subtract(self.selectorTip), .blue);
		rl.drawRectangleRec(rectangleTipTail(self.selectorTail, self.selectorTip), .blue);
	}

	inline fn handleInput(self: *Self, comptime input: rl.KeyboardKey, 
		delta: Seconds) void {
		_ = self;
		_ = delta;
		switch (input) {
			else => {},
		}
	}
};

pub fn main() !void {
	var game: Game = undefined;
	try game.init(allocator);
	defer game.deinit(allocator);
	rl.setTraceLogLevel(.warning);
	rl.setExitKey(.null);
	rl.setTargetFPS(60);
	rl.initWindow(res.w, res.h, "rlzig!");
	defer rl.closeWindow();
	while (!rl.windowShouldClose()) {
		try game.update(allocator, rl.getFrameTime());
		rl.clearBackground(.black);
		rl.beginDrawing();
		defer rl.endDrawing();
		game.draw();
	}
}

fn rectangleV(v: rl.Vector2, u: rl.Vector2) rl.Rectangle {
	return rl.Rectangle { 
		.x		= v.x, 	.y 		= v.y,
		.width	= u.x,	.height = u.y,
	};
}

fn rectangleTipTail(v: rl.Vector2, u: rl.Vector2) rl.Rectangle {
	return rl.Rectangle {
		.x = @min(v.x, u.x),
		.y = @min(v.y, u.y),
		.width = @abs(u.x - v.x),
		.height = @abs(u.y - v.y)
	};
}

fn saveRectsToFile(path: []const u8, recs: []rl.Rectangle) !void {
	const file = try std.fs.cwd().createFile(path, .{});
	defer file.close();
	const stride: usize = @sizeOf([4]f32);
	comptime std.debug.assert(@sizeOf(rl.Rectangle) == stride);
	for (recs) |rec| {
		const a: [stride]u8 = @bitCast(rec);
		_ = try file.write(&a);
	}
}

fn loadRectsFromFile(path: []const u8, gpa: std.mem.Allocator,
	recs: *std.ArrayList(rl.Rectangle)) !void {
	const file = try std.fs.cwd().openFile(path, .{ .mode = .read_only });
	defer file.close();
	const stride: usize = @sizeOf([4]f32);
	comptime std.debug.assert(@sizeOf(rl.Rectangle) == stride);

	const eof: usize = try file.getEndPos();
	var recb: [stride]u8 = undefined;
	if (eof < stride) return;
	var i: usize = 0;
	while (i < eof) : (i += stride) {
		_ = try file.read(&recb);
		try recs.append(gpa, @bitCast(recb));
		try file.seekTo(i);
	}
}
