package org.gameoflife.rules.generator

import org.gameoflife.rules.rulesDSL.Game
import org.gameoflife.rules.rulesDSL.Comparison
import org.gameoflife.rules.rulesDSL.Transition

class RulesGenerator {
	
	def static toJava(Game game)'''
		package GameOfLife;
		
		import java.awt.Point;
		import java.util.ArrayList;
		
		public class RulesOfLife {
			public static void computeInitialConfig(ArrayList<Point> survivingCells) {
				survivingCells.add(new Point(22, 22));
				survivingCells.add(new Point(22, 23));
				survivingCells.add(new Point(22, 24));
				survivingCells.add(new Point(21, 23));
				survivingCells.add(new Point(23, 23));
			}
			
			public static void computeSurvivors(boolean[][] gameBoard, ArrayList<Point> nextLiveCells) {
		        copyGameBoard(gameBoard, nextLiveCells);
		        // Iterate through the array, follow game of life rules
				for (int i = 1; i < gameBoard.length - 1; i++) {
					for (int j = 1; j < gameBoard[0].length - 1; j++) {
						int surrounding = amountOfNeighbors(gameBoard, i, j);

						«FOR rule : game.rules»
						«"\t"»
						if(surrounding «toComparison(rule.condition.comparison)» «rule.condition.number») {
							«toArrayListOperation(rule.transition)»
						}
						«"\n"»
						«ENDFOR»
						
		            }
		        }
			}
		
			private static int amountOfNeighbors(boolean[][] gameBoard, int x, int y) {
				int neighbors = 0;
		
		        if (gameBoard[x-1][y-1]) { neighbors++; }
		        if (gameBoard[x-1][y])   { neighbors++; }
		        if (gameBoard[x-1][y+1]) { neighbors++; }
		        if (gameBoard[x][y-1])   { neighbors++; }
		        if (gameBoard[x][y+1])   { neighbors++; }
		        if (gameBoard[x+1][y-1]) { neighbors++; }
				if (gameBoard[x+1][y])   { neighbors++; }
				if (gameBoard[x+1][y+1]) { neighbors++; }
		
				return neighbors;				
			}
			
			private static void copyGameBoard(boolean[][] gameBoard, ArrayList<Point> nextLiveCells) {
				for(int x = 1; x < gameBoard.length - 1; x++) {
					for(int y = 1; y < gameBoard[0].length - 1; y++) {
						if(gameBoard[x][y])
							nextLiveCells.add(new Point(x - 1, y - 1));
					}
				}
			}
		
		}
	'''
	
	def static toComparison(Comparison c) {
		switch(c) {
			case Comparison::OR_LESS: return '''<='''
			case Comparison::OR_MORE: return '''>='''
			default: return'''=='''
		}
	}
	
	def static toArrayListOperation(Transition t) {
		switch(t) {
			case Transition::TO_ALIVE: return '''nextLiveCells.add(new Point(i - 1, j - 1));
			'''
			case Transition::LIVES: return '''if(gameBoard[i][j])
				nextLiveCells.add(new Point(i - 1, j - 1));
			'''
			case Transition::DIES: return '''nextLiveCells.remove(new Point(i - 1, j - 1));
			'''
		}
	}
}