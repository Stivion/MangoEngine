import MangoEngine

class Bird(MangoEngine.Entity):
    def OnCreate(self):
        self.isJumping = False
        self.jumpForce = (0.0, 40.0)
        self.restartOnNextFrame = True

    def OnFixedUpdate(self):
        # Controls
        if not self.isJumping and MangoEngine.IsKeyPressed(MangoEngine.Keys('Space')):
            super().ApplyForce(self.jumpForce[0], self.jumpForce[1])
            self.isJumping = True
        
        if not MangoEngine.IsKeyPressed(MangoEngine.Keys('Space')):
            self.isJumping = False

    def OnCollisionBegin(self, collidedWith):
        obstacleController = MangoEngine.FindEntityByName('ObstaclesController')
        obstacleController.Restart()
        super().SetPosition(0.0, 0.0)

