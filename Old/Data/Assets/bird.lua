life = 100
dead = false
attack = 100.1
hp = 60.0
str = "hello!"
local speed = 5

GameObject("go")
Script("s")
Animation("a")
Texture("t")

function Start()
    
end

function Update()
   
end

function OnCollisionEnter(collisionData)
   print(collisionData)
end

function OnEnable()
	print("Enabled!")
end

function OnDisable()
	print("Disabled!")
end
