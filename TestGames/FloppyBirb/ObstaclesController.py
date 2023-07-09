import MangoEngine
import random

class ObstaclesController(MangoEngine.Entity):
    def OnCreate(self):
        self.obstacles = []
        self.obstaclesSpeed = 0.02
        self.obstacleBoundaries = (-10, 10)
        self.initialObstaclesCount = 15
        self.obstaclesStep = 1.5
        self.yScaleRange = (4, 6)
        self.yObstaclePosition = 5
        self.windowScale = 1

        self.SpawnInitialObstacles()

    def OnFixedUpdate(self):
        for obstacleIndex, obstaclesPair in enumerate(self.obstacles):
            # Move obstacles
            for obstacle in obstaclesPair:
                currentPosition = obstacle.GetPosition()
                obstacle.SetPosition(currentPosition[0] - self.obstaclesSpeed, currentPosition[1])            

            # Check if obstacles should be moved to the right side
            if obstaclesPair[0].GetPosition()[0] < self.obstacleBoundaries[0]:
                self.ShuffleObstacle(obstacleIndex)
    
    def SpawnObstacle(self, xPosition) -> None:
        topObstacle = MangoEngine.CreateEntity()
        topObstacle.SetRigid(True)
        topObstacle.ConfigureRigidbody(1.0, 1.0, False)
        topObstacle.SetPosition(xPosition, self.yObstaclePosition)

        bottomObstacle = MangoEngine.CreateEntity()
        bottomObstacle.SetRigid(True)
        bottomObstacle.ConfigureRigidbody(1.0, 1.0, False)
        bottomObstacle.SetPosition(xPosition, -self.yObstaclePosition)
        
        self.RandomizeScale(topObstacle, bottomObstacle)
        self.obstacles.append((topObstacle, bottomObstacle))

    def ShuffleObstacle(self, obstacleIndex) -> None:
        lastObstaclesPair = self.obstacles[-1]
        obstaclesPair = self.obstacles[obstacleIndex]

        for index, lastObstacle in enumerate(lastObstaclesPair):
            lastObstaclePosition = lastObstacle.GetPosition()
            obstacle = obstaclesPair[index]
            obstacle.SetPosition(lastObstaclePosition[0] + self.obstaclesStep, lastObstaclePosition[1])

        # Randomize scale
        self.RandomizeScale(obstaclesPair[0], obstaclesPair[1])

        # Move obstacle to the end to maintain correct position
        self.obstacles.remove(obstaclesPair)
        self.obstacles.append(obstaclesPair)

    def SpawnInitialObstacles(self) -> None:
        for i in range(self.initialObstaclesCount):
            xPosition = self.obstacleBoundaries[1] + i * self.obstaclesStep
            self.SpawnObstacle(xPosition)

    def RandomizeScale(self, topObstacle, bottomObstacle) -> None:
        topObstacleScale = random.uniform(self.yScaleRange[0], self.yScaleRange[1])
        topObstacle.SetScale(0.2, topObstacleScale)
        # NOTE: Y-Scale 5.0f is exactly in the middle of the screen, so 10 is combined scale for both obstacles
        bottomObstacleScale = 10 - topObstacleScale - self.windowScale
        bottomObstacle.SetScale(0.2, bottomObstacleScale)

    def Restart(self) -> None:
        for obstaclesPair in self.obstacles:
            for obstacle in obstaclesPair:
                MangoEngine.DestroyEntity(obstacle)

        self.obstacles = []
        self.SpawnInitialObstacles()
